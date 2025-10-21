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
				switch (EquippedWeapon->GetWeaponType()) {
					case EWeaponType::WeaponType_Gun:
					{
						ARangeWeapon* CurrentWeapon = Cast<ARangeWeapon>(EquippedWeapon);
						HUDPackage.CrosshairsCenter = CurrentWeapon->CrosshairsCenter;
						HUDPackage.CrosshairsLeft = CurrentWeapon->CrosshairsLeft;
						HUDPackage.CrosshairsRight = CurrentWeapon->CrosshairsRight;
						HUDPackage.CrosshairsTop = CurrentWeapon->CrosshairsTop;
						HUDPackage.CrosshairsBottom = CurrentWeapon->CrosshairsBottom;
						break;
					}
					
					case EWeaponType::WeaponType_Melee:
					{
						AMeleeWeaponBase* CurrentWeapon = Cast<AMeleeWeaponBase>(EquippedWeapon);
						HUDPackage.CrosshairsCenter = CurrentWeapon->CrosshairsCenter;
						HUDPackage.CrosshairsLeft = CurrentWeapon->CrosshairsLeft;
						HUDPackage.CrosshairsRight = CurrentWeapon->CrosshairsRight;
						HUDPackage.CrosshairsTop = CurrentWeapon->CrosshairsTop;
						HUDPackage.CrosshairsBottom = CurrentWeapon->CrosshairsBottom;
						break;
					}
					

				}
				
			}
			else
			{
				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;
			}
			//¼ÆËã×¼ÐÇÀ©É¢
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

void UCombatComponent::AttackBottonPressed(bool bPressed)
{
	if (EquippedWeapon == nullptr) return;
	bFireButtonPressed = bPressed;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
		FString::Printf(TEXT("Overlap Actor Name is %s"),
			*UEnum::GetDisplayValueAsText(EquippedWeapon->GetWeaponType()).ToString()));
	switch (EquippedWeapon->GetWeaponType()) {
	case EWeaponType::WeaponType_Gun:
		{
			if (CanAttack())
			{
				ServerAttack();

				if (EquippedWeapon)
				{
					CrosshairShootingFactor = 2.f;
				}
			}
			break;
		}
	case EWeaponType::WeaponType_Melee:
		{
			if (CanAttack()) ServerAttack();
			break;
		}

	}


	

}
void UCombatComponent::ServerAttack_Implementation()
{
	if (EquippedWeapon == nullptr) return;
	if (Character->PlayingMantogeState != EPlayingMantoge::PlayingMantoge_Blank && Character->PlayingMantogeState != EPlayingMantoge::PlayingMantoge_Slide) return;
	switch (EquippedWeapon->GetWeaponType()) {
	case EWeaponType::WeaponType_Gun:
		{
			FHitResult HitResult;
			TraceUnderCrosshairs(HitResult);
			MultiCastAttack(HitResult.ImpactPoint);
			break;
		}
	case EWeaponType::WeaponType_Melee:
		{
			Cast<AMeleeWeaponBase>(EquippedWeapon)->HandleAttack();
			MultiCastAttack(FVector(0, 0, 0));
			break;
		}
	}
	
}
void UCombatComponent::MultiCastAttack_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	switch (EquippedWeapon->GetWeaponType()) {
	case EWeaponType::WeaponType_Gun:
		{
			ARangeWeapon* RangerWeapon = Cast<ARangeWeapon>(EquippedWeapon);
			if (RangerWeapon)
			{
				//Ãé×¼×´Ì¬
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
			break;
		}
	case EWeaponType::WeaponType_Melee:
		{
			AMeleeWeaponBase* MeleeWeapon = Cast<AMeleeWeaponBase>(EquippedWeapon);
			Character->SetPlayingMantogeState(EPlayingMantoge::PlayingMantoge_Attack);
			MeleeWeapon->PlayAttackMontage();
		}
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
		switch (EquippedWeapon->GetWeaponType())
		{
			case EWeaponType::WeaponType_Gun:
			{
				CurrentFOV = FMath::FInterpTo(CurrentFOV,
					Cast<ARangeWeapon>(EquippedWeapon)->GetZoomedFov()
					, DeltaTime,
					Cast<ARangeWeapon>(EquippedWeapon)->GetZoomInterSpeed());
				break;
			}
		}
		

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
	CarriedAmmoMap.Emplace(EWeaponType::WeaponType_None,0);

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
	if (Character->PlayingMantogeState == EPlayingMantoge::PlayingMantoge_Reload)
	{
		Character->SetPlayingMantogeState(EPlayingMantoge::PlayingMantoge_Blank);
		if (Character->HasAuthority())
		{
			UpdateAmmoValues();
		}
	}

}
void UCombatComponent::EquipWeapon(AWeaponBase* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr) return;
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	switch (EquippedWeapon->GetWeaponType())
	{
		case EWeaponType::WeaponType_Gun:
		{
			const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName("RightHandSocket");
			if (HandSocket)
			{
				HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
				Character->SetWeaponType(EquippedWeapon->WeaponType);
			}
			EquippedWeapon->SetOwner(Character);
			if (Cast<ARangeWeapon>(EquippedWeapon))
			{
				ARangeWeapon* RangeWeapon = Cast<ARangeWeapon>(EquippedWeapon);
				/*Character->GetCharacterMovement()->MaxWalkSpeed = RangeWeapon->GetRifleWalkSpeed();*/
				Controller = Controller == nullptr ? Cast<ACowBoyPlayerController>(Character->GetController()) : Controller;
				if (CarriedAmmoMap.Contains(RangeWeapon->WeaponType))
				{
					CarriedAmmo = CarriedAmmoMap[RangeWeapon->WeaponType];
				}
				else
				{
					CarriedAmmo = 0;
				}
				if (Controller)
				{
					Controller->SetRangeWeaponHUDVisible(true);
					RangeWeapon->SetHUDAmmo();
					Controller->SetHUDCarriedAmmo(CarriedAmmo);
				}
			}
			break;
		}

		case EWeaponType::WeaponType_Melee:
		{

			const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName("RightHandMeleeSocket");
			if (HandSocket)
			{
				HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
				Character->SetWeaponType(EquippedWeapon->WeaponType);
			}
			EquippedWeapon->SetOwner(Character);
			Controller = Controller == nullptr ? Cast<ACowBoyPlayerController>(Character->GetController()) : Controller;
			Controller->SetMeleeWeaponHUDVisible(true);
			break;
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

void UCombatComponent::UnEquipWeapon()
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_PickUp);
	Controller = Controller == nullptr ? Cast<ACowBoyPlayerController>(Character->GetController()) : Controller;
	switch (EquippedWeapon->GetWeaponType())
	{
		case EWeaponType::WeaponType_Gun:
		{
			const USkeletalMeshSocket* HolsterSocket = Character->GetMesh()->GetSocketByName("HolsterSocket");
			if (HolsterSocket)
			{
				HolsterSocket->AttachActor(EquippedWeapon, Character->GetMesh());
				Character->SetWeaponType(EWeaponType::WeaponType_None);
			}
		
			
			if(Controller) Controller->SetRangeWeaponHUDVisible(false);
			break;
		}

		case EWeaponType::WeaponType_Melee:
		{

			const USkeletalMeshSocket* HolsterSocket = Character->GetMesh()->GetSocketByName("HolsterMeleeSocket");
			if (HolsterSocket)
			{
				HolsterSocket->AttachActor(EquippedWeapon, Character->GetMesh());
				Character->SetWeaponType(EWeaponType::WeaponType_None);
			}
			if (Controller) Controller->SetMeleeWeaponHUDVisible(false);
			break;
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
	EquippedWeapon = nullptr;
}

bool UCombatComponent::CanAttack()
{
	if (EquippedWeapon == nullptr) return false;
	switch (EquippedWeapon->GetWeaponType()) {
	case EWeaponType::WeaponType_Gun:
		if (Cast<ARangeWeapon>(EquippedWeapon))
		{
			ARangeWeapon* RangeWeapon = Cast<ARangeWeapon>(EquippedWeapon);
			return RangeWeapon->CanAttack();
		}
		break;
	case EWeaponType::WeaponType_Melee:
		return true;
	}
	
	return false;
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

