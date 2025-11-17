// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/LagCompenstionComponent.h"
#include "Character/CowBoyCharacter.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Westword.h"

ULagCompenstionComponent::ULagCompenstionComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;

	
}





void ULagCompenstionComponent::BeginPlay()
{
	Super::BeginPlay();


}

void ULagCompenstionComponent::SaveFramePackage(FFramPackage& Package)
{
	Character = Character == nullptr ? Cast<ACowBoyCharacter>(GetOwner()) : Character;
	if (Character)
	{
		Package.Time = GetWorld()->GetTimeSeconds();
		for (auto& BoxPair : Character->HitCollisionBoxes)
		{
			FBoxInformation BoxInformation;
			BoxInformation.Location = BoxPair.Value->GetComponentLocation();
			BoxInformation.Rotation = BoxPair.Value->GetComponentRotation();
			BoxInformation.BoxExtent = BoxPair.Value->GetScaledBoxExtent();
			Package.HitBoxInfo.Add(BoxPair.Key, BoxInformation);

		}
	}
}

FServerSideRewindResult ULagCompenstionComponent::ServerSideRewind(ACowBoyCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime)
{
	FFramPackage FrameToCheck = GetFrameToCheck(HitCharacter, HitTime);
	return(ConfirmHit(FrameToCheck, HitCharacter, TraceStart, HitLocation));
}


FServerSideRewindResult ULagCompenstionComponent::ProjectileServerSideRewind(ACowBoyCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime)
{
	FFramPackage FrameToCheck = GetFrameToCheck(HitCharacter, HitTime);
	return(ProjectileConfirmHit(FrameToCheck,HitCharacter,  TraceStart, InitialVelocity, HitTime));
}



void ULagCompenstionComponent::ServerScoreRequest_Implementation(ACowBoyCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime, AWeaponBase* DamageCauser)
{
	FServerSideRewindResult Confirm = ServerSideRewind(HitCharacter, TraceStart, HitLocation, HitTime);
	if (Character && HitCharacter && Confirm.bHitConfirmed)
	{
		UGameplayStatics::ApplyDamage(
			HitCharacter,
			30,
			Character->Controller,
			DamageCauser,
			UDamageType::StaticClass()
			);
	}
}

void ULagCompenstionComponent::ProjectileServerScoreRequest_Implementation(ACowBoyCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime)
{
	FServerSideRewindResult Confirm = ProjectileServerSideRewind(HitCharacter, TraceStart, InitialVelocity, HitTime);
	if (Character && HitCharacter && Confirm.bHitConfirmed)
	{
		float WeaponDamage = 0.f;
		float WeaponHeadShotDamage = 0.f;
		if (Cast<ARangeWeapon>(Character->GetCombatComponent()->GetEquippedWeapon()))
		{
			WeaponDamage = Cast<ARangeWeapon>(Character->GetCombatComponent()->GetEquippedWeapon())->GetProjectileDamage();
			WeaponHeadShotDamage = Cast<ARangeWeapon>(Character->GetCombatComponent()->GetEquippedWeapon())->GetProjectileHeadShotDamage();
		}
		const float Damage = Confirm.bHeadShot ? (HitCharacter->GetHealth()+1) : WeaponDamage;
		HitCharacter->SetShootHead(Confirm.bHeadShot);
		UGameplayStatics::ApplyDamage(
			HitCharacter,
			Damage,
			Character->Controller,
			Character->GetCombatComponent()->GetEquippedWeapon(),
			UDamageType::StaticClass()
		);
	}
}

FFramPackage ULagCompenstionComponent::InterpBetweenFrames(const FFramPackage& OlderFrame, const FFramPackage& YoungerFrame, float HitTime)
{

	const float Distance = YoungerFrame.Time - OlderFrame.Time;
	const float InterpFrraction = FMath::Clamp((HitTime - OlderFrame.Time) / Distance,0.f,1.f);
	FFramPackage InterpFramePackage;
	InterpFramePackage.Time = HitTime;
	for (auto& YoungerPair : YoungerFrame.HitBoxInfo)
	{
		const FName& BoxInfoName = YoungerPair.Key;
		const FBoxInformation& OlderBox = OlderFrame.HitBoxInfo[BoxInfoName];
		const FBoxInformation& YoungerBox = YoungerFrame.HitBoxInfo[BoxInfoName];
		FBoxInformation InterpBoxInfo;
		InterpBoxInfo.Location = FMath::VInterpTo(OlderBox.Location, YoungerBox.Location, 1.f, InterpFrraction);
		InterpBoxInfo.Rotation = FMath::RInterpTo(OlderBox.Rotation, YoungerBox.Rotation, 1.f, InterpFrraction);
		InterpBoxInfo.BoxExtent = YoungerBox.BoxExtent;
		InterpFramePackage.HitBoxInfo.Add(BoxInfoName, InterpBoxInfo);
	}
	return InterpFramePackage;
}

FServerSideRewindResult ULagCompenstionComponent::ConfirmHit(const FFramPackage& Package, ACowBoyCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation)
{
	if(HitCharacter == nullptr) return  FServerSideRewindResult();
	FFramPackage CurrentFramePackage;
	CacheBoxPosition(HitCharacter, CurrentFramePackage);
	MoveBoxes(HitCharacter, Package);
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::NoCollision);

	//先启用头部碰撞
	UBoxComponent* HeadBox = HitCharacter->HitCollisionBoxes[FName("head")];
	HeadBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HeadBox->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
	FHitResult ConfirmHitResult;
	const FVector TraceEnd = TraceStart + (HitLocation - TraceStart) * 1.25f;
	if (GetWorld())
	{
		GetWorld()->LineTraceSingleByChannel(
			ConfirmHitResult,
			TraceStart,
			TraceEnd,
			ECC_HitBox
		);
		if (ConfirmHitResult.bBlockingHit)//爆头
		{
			if (ConfirmHitResult.Component.IsValid())
			{
				UBoxComponent* Box = Cast<UBoxComponent>(ConfirmHitResult.Component);
				if (Box)
				{
					DrawDebugBox(GetWorld(), Box->GetComponentLocation(), Box->GetScaledBoxExtent(), FQuat(Box->GetComponentRotation()),FColor::Red,false,8.f);
				}
			}
			ResetHitBoxes(HitCharacter, CurrentFramePackage);
			EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryOnly);
			return FServerSideRewindResult(true, true);
		}
		else//未爆头，检查其他盒子
		{
			for (auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
			{
				if (HitBoxPair.Value != nullptr)
				{
					HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					HitBoxPair.Value->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
				}
			}

			GetWorld()->LineTraceSingleByChannel(
				ConfirmHitResult,
				TraceStart,
				TraceEnd,
				ECC_HitBox
			);
			if (ConfirmHitResult.bBlockingHit)
			{
				if (ConfirmHitResult.Component.IsValid())
				{
					UBoxComponent* Box = Cast<UBoxComponent>(ConfirmHitResult.Component);
					if (Box)
					{
						DrawDebugBox(GetWorld(), Box->GetComponentLocation(), Box->GetScaledBoxExtent(), FQuat(Box->GetComponentRotation()), FColor::Blue, false, 8.f);
					}
				}
				ResetHitBoxes(HitCharacter, CurrentFramePackage);
				EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryOnly);
				return FServerSideRewindResult(true,false);
			}
		}
	}
	//未命中
	ResetHitBoxes(HitCharacter, CurrentFramePackage);
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryOnly);
	return FServerSideRewindResult(false, false);
}

FServerSideRewindResult ULagCompenstionComponent::ProjectileConfirmHit(const FFramPackage& Package,ACowBoyCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime)
{
	if (HitCharacter == nullptr) return  FServerSideRewindResult();
	FFramPackage CurrentFramePackage;
	CacheBoxPosition(HitCharacter, CurrentFramePackage);
	MoveBoxes(HitCharacter, Package);
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::NoCollision);

	//先启用头部碰撞
	UBoxComponent* HeadBox = HitCharacter->HitCollisionBoxes[FName("head")];
	HeadBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HeadBox->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
	FHitResult ConfirmHitResult;

	FPredictProjectilePathParams PathParams;
	PathParams.bTraceWithCollision = true;
	PathParams.MaxSimTime = MaxRecordTime;
	PathParams.LaunchVelocity = InitialVelocity;
	PathParams.StartLocation = TraceStart;
	PathParams.ProjectileRadius = 5.f;
	PathParams.SimFrequency = 15.f;
	PathParams.TraceChannel = ECC_HitBox;
	PathParams.ActorsToIgnore.Add(GetOwner());
	PathParams.DrawDebugTime = 5.f;
	PathParams.DrawDebugType = EDrawDebugTrace::ForDuration;

	FPredictProjectilePathResult PathResult;
	UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);
	
	if (PathResult.HitResult.bBlockingHit)//hit the head,return early
	{
		if (PathResult.HitResult.Component.IsValid())
		{
			UBoxComponent* Box = Cast<UBoxComponent>(PathResult.HitResult.Component);
			if (Box)
			{
				DrawDebugBox(GetWorld(), Box->GetComponentLocation(), Box->GetScaledBoxExtent(), FQuat(Box->GetComponentRotation()), FColor::Red, false, 8.f);
			}
		}
		ResetHitBoxes(HitCharacter, CurrentFramePackage);
		EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryOnly);
		return FServerSideRewindResult(true, true);
	}
	else //没爆头
	{
		for (auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
		{
			if (HitBoxPair.Value != nullptr)
			{
				HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				HitBoxPair.Value->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
			}
		}

		UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);
		if (PathResult.HitResult.bBlockingHit)
		{
			if (PathResult.HitResult.Component.IsValid())
			{
				UBoxComponent* Box = Cast<UBoxComponent>(PathResult.HitResult.Component);
				if (Box)
				{
					DrawDebugBox(GetWorld(), Box->GetComponentLocation(), Box->GetScaledBoxExtent(), FQuat(Box->GetComponentRotation()), FColor::Blue, false, 8.f);
				}
			}
			ResetHitBoxes(HitCharacter, CurrentFramePackage);
			EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryOnly);
			return FServerSideRewindResult(true, false);
		}

	}
	//未命中
	ResetHitBoxes(HitCharacter, CurrentFramePackage);
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryOnly);
	return FServerSideRewindResult(false, false);
}

void ULagCompenstionComponent::CacheBoxPosition(ACowBoyCharacter* HitCharacter, FFramPackage& OutFramePackage)
{
	if (HitCharacter == nullptr) return;
	for (auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
	{
		if (HitBoxPair.Value != nullptr)
		{
			FBoxInformation BoxInfo;
			BoxInfo.Location = HitBoxPair.Value->GetComponentLocation();
			BoxInfo.Rotation = HitBoxPair.Value->GetComponentRotation();
			BoxInfo.BoxExtent = HitBoxPair.Value->GetScaledBoxExtent();
			OutFramePackage.HitBoxInfo.Add(HitBoxPair.Key, BoxInfo);
		}
	}
}

void ULagCompenstionComponent::MoveBoxes(ACowBoyCharacter* HitCharacter, const FFramPackage& FramePackage)
{
	if (HitCharacter == nullptr) return;
	for (auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
	{
		if (HitBoxPair.Value != nullptr)
		{
			HitBoxPair.Value->SetWorldLocation(FramePackage.HitBoxInfo[HitBoxPair.Key].Location);
			HitBoxPair.Value->SetWorldRotation(FramePackage.HitBoxInfo[HitBoxPair.Key].Rotation);
			HitBoxPair.Value->SetBoxExtent(FramePackage.HitBoxInfo[HitBoxPair.Key].BoxExtent);

		}
	}
}

void ULagCompenstionComponent::ResetHitBoxes(ACowBoyCharacter* HitCharacter, const FFramPackage& FramePackage)
{
	if (HitCharacter == nullptr) return;
	for (auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
	{
		if (HitBoxPair.Value != nullptr)
		{
			HitBoxPair.Value->SetWorldLocation(FramePackage.HitBoxInfo[HitBoxPair.Key].Location);
			HitBoxPair.Value->SetWorldRotation(FramePackage.HitBoxInfo[HitBoxPair.Key].Rotation);
			HitBoxPair.Value->SetBoxExtent(FramePackage.HitBoxInfo[HitBoxPair.Key].BoxExtent);
			HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		}
	}

}

void ULagCompenstionComponent::EnableCharacterMeshCollision(ACowBoyCharacter* CowBoyCharacter, ECollisionEnabled::Type CollisionEnableType)
{
	if (CowBoyCharacter && CowBoyCharacter->GetMesh())
	{
		CowBoyCharacter->GetMesh()->SetCollisionEnabled(CollisionEnableType);
	}
}



void ULagCompenstionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	SaveFramePackage();
	
}

void ULagCompenstionComponent::SaveFramePackage()
{
	if (Character == nullptr || !Character->HasAuthority()) return;
	if (FrameHistory.Num() <= 1)
	{
		FFramPackage ThisFrame;
		SaveFramePackage(ThisFrame);
		FrameHistory.AddHead(ThisFrame);
	}
	else
	{
		float HistoryLength = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		while (HistoryLength > MaxRecordTime)
		{
			FrameHistory.RemoveNode(FrameHistory.GetTail());
			HistoryLength = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		}
		FFramPackage ThisFrame;
		SaveFramePackage(ThisFrame);
		FrameHistory.AddHead(ThisFrame);
		/*ShowFramePackage(ThisFrame, FColor::Red);*/
	}
}

FFramPackage ULagCompenstionComponent::GetFrameToCheck(ACowBoyCharacter* HitCharacter, float HitTime)
{
	bool bReturn =
		HitCharacter == nullptr ||
		HitCharacter->GetLagCompensation() == nullptr ||
		HitCharacter->GetLagCompensation()->FrameHistory.GetHead() == nullptr ||
		HitCharacter->GetLagCompensation()->FrameHistory.GetTail() == nullptr;
	if (bReturn) return FFramPackage();
	FFramPackage FrameToCheck;
	bool bShouldeInterpolate = true;
	//Frame history of the HitCharacter
	const TDoubleLinkedList<FFramPackage>& History = HitCharacter->GetLagCompensation()->FrameHistory;
	const float OldestHistoryTIme = History.GetTail()->GetValue().Time;
	const float NewestHistoryTIme = History.GetHead()->GetValue().Time;
	if (OldestHistoryTIme > HitTime)
	{
		//延迟过高无法倒带
		return FFramPackage();
	}
	if (OldestHistoryTIme > HitTime)
	{
		FrameToCheck = History.GetTail()->GetValue();
		bShouldeInterpolate = false;
	}
	if (NewestHistoryTIme <= HitTime)
	{
		FrameToCheck = History.GetHead()->GetValue();
		bShouldeInterpolate = false;
	}
	TDoubleLinkedList<FFramPackage>::TDoubleLinkedListNode* Younger = History.GetHead();
	TDoubleLinkedList<FFramPackage>::TDoubleLinkedListNode* Older = Younger;
	while (Older->GetValue().Time > HitTime)
	{
		//March back until OlderTime < HitTime < YougerTime
		if (Older->GetNextNode() == nullptr)break;
		Older = Older->GetNextNode();
		if (Older->GetValue().Time > HitTime)
		{
			Younger = Older;
		}
	}
	if (Older->GetValue().Time == HitTime)
	{
		FrameToCheck = Older->GetValue();
		bShouldeInterpolate = false;
	}

	if (bShouldeInterpolate)
	{
		//HitTime在两个记录帧中间，需要插值
		FrameToCheck = InterpBetweenFrames(Older->GetValue(), Younger->GetValue(), HitTime);
	}
	return FrameToCheck;
}



void ULagCompenstionComponent::ShowFramePackage(const FFramPackage& package, const FColor& Color)
{
	for (auto& BoxInfo : package.HitBoxInfo)
	{
		DrawDebugBox(
			GetWorld(),
			BoxInfo.Value.Location,
			BoxInfo.Value.BoxExtent,
			FQuat(BoxInfo.Value.Rotation),
			Color,
			false,
			4.f
		);
	}
}

