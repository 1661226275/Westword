// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Weapon/RangeWeapon.h"
#include "Character/CowBoyCharacter.h"
#include "Engine/StaticMeshSocket.h"
#include "Projectile.h"
#include "Character/CowBoyCharacter.h"
#include "PlayerController/CowBoyPlayerController.h"
void ARangeWeapon::PlayFireMontage(bool bIsAiming)
{
	ACowBoyCharacter* Character = Cast<ACowBoyCharacter>(GetOwner());
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (AnimInstance && FireMontage)
	{
		AnimInstance->Montage_Play(FireMontage);
		FName SectionName;
		SectionName = bIsAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName, FireMontage);
	}
}

void ARangeWeapon::Fire(const FVector& HitTarget)
{
	if (!HasAuthority()) return;
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	if (ProjectileClass == nullptr)return;
	const UStaticMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName("FireSocket");
	if (MuzzleSocket)
	{
		FTransform SocketTransform;
		MuzzleSocket->GetSocketTransform(SocketTransform,GetWeaponMesh());
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = InstigatorPawn;
		GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
	}
	SpendRound();
}

void ARangeWeapon::PlayReloadMontage()
{
	ACowBoyCharacter* Character = Cast<ACowBoyCharacter>(GetOwner());
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName ;
		switch (WeaponType)
		{
		case EWeaponType::WeaponType_Gun:
			SectionName = FName("Reload");
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, ReloadMontage);
		Character->SetPlayingMantogeState(EPlayingMantoge::PlayingMantoge_Reload);
	}
}

void ARangeWeapon::PlayEquipMontage()
{
	Super::PlayEquipMontage();
	ACowBoyCharacter* Character = Cast<ACowBoyCharacter>(GetOwner());
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (AnimInstance != nullptr && (Character->PlayingMantogeState == EPlayingMantoge::PlayingMantoge_Blank ||
		Character->PlayingMantogeState == EPlayingMantoge::PlayingMantoge_Slide))
	{
		AnimInstance->Montage_Play(EquipAnimMontage, 1.f);
		FName SectionName = FName("EquipRange");
		AnimInstance->Montage_JumpToSection(SectionName, EquipAnimMontage);
		Character->SetPlayingMantogeState(EPlayingMantoge::PlayingMantoge_EquipWeapon);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("RangeWeapon Playing Equip Montage")));
		}
	}
}

void ARangeWeapon::PlayUnEquipMontage()
{
	Super::PlayUnEquipMontage();
	ACowBoyCharacter* Character = Cast<ACowBoyCharacter>(GetOwner());
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (AnimInstance != nullptr && (Character->PlayingMantogeState == EPlayingMantoge::PlayingMantoge_Blank ||
		Character->PlayingMantogeState == EPlayingMantoge::PlayingMantoge_Slide))
	{
		AnimInstance->Montage_Play(UnequipAnimMontage, 1.f);
		FName SectionName = FName("UnEquipRange");
		AnimInstance->Montage_JumpToSection(SectionName, UnequipAnimMontage);
		Character->SetPlayingMantogeState(EPlayingMantoge::PlayingMantoge_UnEquipWeapon);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("RangeWeapon Playing UnEquip Montage")));
		}
	}
}

void ARangeWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARangeWeapon, Ammo);
}

void ARangeWeapon::SetHUDAmmo()
{
	CowBoyOwnerCharacter = CowBoyOwnerCharacter == nullptr ? Cast<ACowBoyCharacter>(GetOwner()) : CowBoyOwnerCharacter;
	if (CowBoyOwnerCharacter)
	{
		CowBoyOwnerController = CowBoyOwnerController == nullptr ? Cast<ACowBoyPlayerController>(CowBoyOwnerCharacter->GetController()) : CowBoyOwnerController;
		if (CowBoyOwnerController)
		{
			CowBoyOwnerController->SetHUDWeaponAmmo(Ammo);
		}
	}
}

void ARangeWeapon::SetWeaponHUDVisible(bool bIsVisible)
{
	CowBoyOwnerCharacter = CowBoyOwnerCharacter == nullptr ? Cast<ACowBoyCharacter>(GetOwner()) : CowBoyOwnerCharacter;
	if (CowBoyOwnerCharacter)
	{
		CowBoyOwnerController = CowBoyOwnerController == nullptr ? Cast<ACowBoyPlayerController>(CowBoyOwnerCharacter->GetController()) : CowBoyOwnerController;
	}
}


void ARangeWeapon::AddAmmo(int32 AmmoToAdd)
{
	Ammo = FMath::Clamp(Ammo - AmmoToAdd, 0, MagCapacity);
	SetHUDAmmo();
}



void ARangeWeapon::SpendRound()
{
	--Ammo;
	SetHUDAmmo();
}

void ARangeWeapon::OnRep_Ammo()
{
	SetHUDAmmo();
}

void ARangeWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
	if (Owner == nullptr)
	{
		CowBoyOwnerCharacter = nullptr;
		CowBoyOwnerController = nullptr;
	}
	else
	{
		EWeaponState OutState;
		GetWeaponState(OutState);
		if (OutState == EWeaponState::EWS_Equipped)
		{
			SetHUDAmmo();
		}
		
	}
	
}


