// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CombatComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Character/CowBoyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Westword/PlayerController/CowBoyPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Sound/SoundCue.h"



// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}



// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	if (Character->GetThirdViewCamera())
	{
		DefaultFOV = Character->GetThirdViewCamera()->FieldOfView;
		CurrentFOV = DefaultFOV;
	}
	if(Character->HasAuthority())
	{
		InitializeCarriedAmmo();
	}

}

// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Character && Character->IsLocallyControlled())
	{
		SetHUDCrosshairs(DeltaTime);
		InterFov(DeltaTime);

		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;
	}
	
}
void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (Character == nullptr || Character->GetController() == nullptr) return;
	Controller = Controller == nullptr ? Cast<ACowBoyPlayerController>(Character->GetController()) : Controller;
	if (Controller)
	{
		HUD = HUD == nullptr ? Cast<ACowBoyHUD>(Controller->GetHUD()) : HUD;
		if (HUD)
		{
			if (EquippedWeapon)
			{
				ARangeWeapon* CurrentWeapon = Cast<ARangeWeapon>(EquippedWeapon);
				HUDPackage.CrosshairsCenter = CurrentWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = CurrentWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight = CurrentWeapon->CrosshairsRight;
				HUDPackage.CrosshairsTop = CurrentWeapon->CrosshairsTop;
				HUDPackage.CrosshairsBottom = CurrentWeapon->CrosshairsBottom;
			}
			else
			{
				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;
			}
			//计算准星扩散
			//[0-600] -> [0-1]
			FVector2D WalkSpeedRange(0.f, Cast<UCharacterMovementComponent>(Character->GetMovementComponent())->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.f, 1.f);
			FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0.f;
			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

			if (Character->GetCharacterMovement()->IsFalling())
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
			}
			else
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
			}
			if (Character->IsAiming())
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.58f, DeltaTime, 30.f);
			}
			else
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
			}

			CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 40.f);

			HUDPackage.CrosshairSpread =
				0.5f +
				CrosshairVelocityFactor +
				CrosshairInAirFactor -
				CrosshairAimFactor +
				CrosshairShootingFactor;
			HUD->SetHUDPackage(HUDPackage);
		}
	}
}

void UCombatComponent::FireBottonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	
	if(CanFire())
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		ServerFire(HitResult.ImpactPoint);

		if (EquippedWeapon)
		{
			CrosshairShootingFactor = 2.f;
		}
	}

}
void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MultiCastFire(TraceHitTarget);
}
void UCombatComponent::MultiCastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	//开火条件判断：
	//1.持有武器
	//2.PlayingMantogeState为空闲或者滑铲
	if (EquippedWeapon == nullptr) return;
	if (Character->PlayingMantogeState != EPlayingMantoge::PlayingMantoge_Blank && Character->PlayingMantogeState != EPlayingMantoge::PlayingMantoge_Slide) return;
	ARangeWeapon* RangerWeapon = Cast<ARangeWeapon>(EquippedWeapon);
	if (RangerWeapon)
	{
		//瞄准状态
		if (Character->IsAiming())
		{
			RangerWeapon->PlayFireMontage(true);
		}
		else
		{
			RangerWeapon->PlayFireMontage(false);
		}
		Character->SetPlayingMantogeState(EPlayingMantoge::PlayingMantoge_Attack);
		RangerWeapon->Fire(TraceHitTarget);
	}

}



void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	FVector2D CrosshairLocation = FVector2D(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(GetWorld(), 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);
	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;

		if (Character)
		{
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);
		}

		FVector End = Start + CrosshairWorldDirection * 50000.f;
		bool TraceResult = GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECC_Visibility
		);
		if (!TraceResult)
		{
			TraceHitResult.ImpactPoint = FVector_NetQuantize(End);
		}
		if (TraceHitResult.bBlockingHit)
		{
			//DrawDebugSphere(GetWorld(), TraceHitResult.ImpactPoint, 16.f, 12, FColor::Red, false);
		}
		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairsInterface>())
		{
			
			HUDPackage.CrosshairColor = FLinearColor::Red;
		}
		else
		{
			HUDPackage.CrosshairColor = FLinearColor::White;
		}
	}
}



void UCombatComponent::InterFov(float DeltaTime)
{
	if (EquippedWeapon == nullptr) return;
	if (Character->IsAiming())
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, 
			Cast<ARangeWeapon>(EquippedWeapon)->GetZoomedFov()
			, DeltaTime, 
			Cast<ARangeWeapon>(EquippedWeapon)->GetZoomInterSpeed());

	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterSpeed);
	}
	if (Character->GetThirdViewCamera())
	{
		Character->GetThirdViewCamera()->SetFieldOfView(CurrentFOV);
	}
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	Controller = Controller == nullptr ? Cast<ACowBoyPlayerController>(Character->GetController()) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
}

void UCombatComponent::InitializeCarriedAmmo()
{
	CarriedAmmoMap.Emplace(EWeaponType::WeaponType_Gun, StatrtingRoundAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::WeaponType_Melee, 0);

}



void UCombatComponent::Reload()
{
	if (EquippedWeapon == nullptr) return;
	if (Character->PlayingMantogeState != EPlayingMantoge::PlayingMantoge_Blank && Character->PlayingMantogeState != EPlayingMantoge::PlayingMantoge_Slide) return;
	if(CarriedAmmo > 0 && EquippedWeapon)
	{
		ServerReload();
	}
}


void UCombatComponent::ServerReload_Implementation()
{
	UpdateAmmoValues();
	//
	MultiCastReload();
}

void UCombatComponent::MultiCastReload_Implementation()
{
	if (EquippedWeapon == nullptr) return;
	ARangeWeapon* RangeWeapon = Cast<ARangeWeapon>(EquippedWeapon);
	if (RangeWeapon)
	{
		RangeWeapon->PlayReloadMontage();
		Character->SetPlayingMantogeState(EPlayingMantoge::PlayingMantoge_Reload);
	}
}

int UCombatComponent::AmountToReload()
{
	if (EquippedWeapon == nullptr) return 0;
	ARangeWeapon* RangeWeapon = Cast<ARangeWeapon>(EquippedWeapon);
	if (RangeWeapon)
	{
		int32 RoomInMag = RangeWeapon->GetMagCapacity() - RangeWeapon->GetAmmo();

		if (CarriedAmmoMap.Contains(RangeWeapon->WeaponType))
		{
			int32 AmountCarried = CarriedAmmoMap[RangeWeapon->WeaponType];
			int32 Least = FMath::Min(RoomInMag, AmountCarried);
			return FMath::Clamp(RoomInMag, 0, Least);
		}
	}
	return 0;
}

void UCombatComponent::UpdateAmmoValues()
{
	int32 ReloadAmount = AmountToReload();
	if (CarriedAmmoMap.Contains(Cast<ARangeWeapon>(EquippedWeapon)->WeaponType))
	{
		CarriedAmmoMap[Cast<ARangeWeapon>(EquippedWeapon)->WeaponType] -= ReloadAmount;
		CarriedAmmo = CarriedAmmoMap[Cast<ARangeWeapon>(EquippedWeapon)->WeaponType];
	}

	Controller = Controller == nullptr ? Cast<ACowBoyPlayerController>(Character->GetController()) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}

	Cast<ARangeWeapon>(EquippedWeapon)->AddAmmo(-ReloadAmount);
}
void UCombatComponent::FinishReloading()
{
	Character->SetPlayingMantogeState(EPlayingMantoge::PlayingMantoge_Blank);
	if(Character->HasAuthority())
	{
		UpdateAmmoValues();
	}
}
void UCombatComponent::EquipWeapon(AWeaponBase* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr) return;
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName("RightHandSocket");
	if (HandSocket)
	{
		if (HandSocket->AttachActor(EquippedWeapon, Character->GetMesh()))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Attach Success")));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Attach Fail")));
		}
		Character->SetWeaponType(EquippedWeapon->WeaponType);
	}
	EquippedWeapon->SetOwner(Character);
	if(Cast<ARangeWeapon>(EquippedWeapon))
	{
		ARangeWeapon* RangeWeapon = Cast<ARangeWeapon>(EquippedWeapon);
		/*Character->GetCharacterMovement()->MaxWalkSpeed = RangeWeapon->GetRifleWalkSpeed();*/
		RangeWeapon->SetHUDAmmo();
		if(CarriedAmmoMap.Contains(RangeWeapon->WeaponType))
		{
			CarriedAmmo = CarriedAmmoMap[RangeWeapon->WeaponType];
		}
		else
		{
			CarriedAmmo = 0;
		}
		Controller = Controller == nullptr ? Cast<ACowBoyPlayerController>(Character->GetController()) : Controller;
		if (Controller)
		{
			Controller->SetHUDCarriedAmmo(CarriedAmmo);
		}
	}
	if (EquippedWeapon->EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EquippedWeapon->EquipSound,
			Character->GetActorLocation()
		);
	}

}

bool UCombatComponent::CanFire()
{
	if (EquippedWeapon == nullptr) return false;
	if(Cast<ARangeWeapon>(EquippedWeapon))
	{
		ARangeWeapon* RangeWeapon = Cast<ARangeWeapon>(EquippedWeapon);
		return RangeWeapon->CanFire();
	}
	return true;
}



void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, Player_State);
	DOREPLIFETIME_CONDITION(UCombatComponent, CarriedAmmo, COND_OwnerOnly);
}

void UCombatComponent::SetPlayerState(ECharacterState NewState)
{
	Player_State = NewState;
	ServerSetPlayerState(NewState);

}

void UCombatComponent::ServerSetPlayerState_Implementation(ECharacterState NewState)
{
	Player_State = NewState;
}

