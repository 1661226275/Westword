// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Weapon/RangeWeapon.h"
#include "Character/CowBoyCharacter.h"
#include "Engine/StaticMeshSocket.h"
#include "Projectile.h"
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
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("No MuzzleSocket")));
	}
}

void ARangeWeapon::PlayReloadMontage()
{
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
		Character->SetPlayingMantogeState(EPlayingMantoge::PlayingMantoge_UnEquipWeapon);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("RangeWeapon Playing UnEquip Montage")));
		}
	}
}
