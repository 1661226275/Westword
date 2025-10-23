// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Weapon/RangeWeapon.h"
#include "Character/CowBoyCharacter.h"
#include "Engine/StaticMeshSocket.h"
#include "Projectile.h"
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
	SpendRound();
	
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	if (ProjectileClass == nullptr)return;
	const UStaticMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName("FireSocket");
	UWorld* World = GetWorld();
	if (MuzzleSocket && World)
	{
		FTransform SocketTransform;
		MuzzleSocket->GetSocketTransform(SocketTransform,GetWeaponMesh());
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = InstigatorPawn;
		AProjectile* SpawnedProjectile = nullptr;
		if (bUseServerSidleRewind)
		{
			if (InstigatorPawn->HasAuthority())//server
			{
				if (InstigatorPawn->IsLocallyControlled()) //server,host - use replicated projectile
				{
					SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
					SpawnedProjectile->bUseServerSideRewind = false;
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, FString("server,host - use replicated projectile"));
					
				}
				else //server,not locally controlled spawn non-replicated projectile,ssr
				{
					SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
					SpawnedProjectile->bUseServerSideRewind = true;
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, FString("server,not locally controlled spawn non-replicated projectile,ssr"));
				}

			}
			else//client using ssr
			{
				if (InstigatorPawn->IsLocallyControlled()) //client,locallt controlled -spawn non-repliced projectile,use ssr
				{
					SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
					SpawnedProjectile->bUseServerSideRewind = true;
					SpawnedProjectile->TraceStart = SocketTransform.GetLocation();
					SpawnedProjectile->InitialVelocity = SpawnedProjectile->GetActorForwardVector() * SpawnedProjectile->InitialSpeed;
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, FString("client,locallt controlled -spawn non-repliced projectile,use ssr"));

				}
				else
				{
					SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
					SpawnedProjectile->bUseServerSideRewind = false;
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, FString("other"));

				}
			}
		}
		else //weapon not using ssr
		{
			if (InstigatorPawn->HasAuthority())
			{
				SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
				SpawnedProjectile->bUseServerSideRewind = false;
			}
		}	
	}
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










void ARangeWeapon::SpendRound()
{
	--Ammo;
	SetHUDAmmo();
	if (HasAuthority())
	{
		ClientUpdateAmmo(Ammo);
	}
	else
	{
		++Sequence;
	}
}


void ARangeWeapon::ClientUpdateAmmo_Implementation(int32 ServerAmmo)
{
	if (HasAuthority()) return;
	Ammo = ServerAmmo;
	--Sequence;
	Ammo -= Sequence;
	SetHUDAmmo();
}

void ARangeWeapon::ClientAddAmmo_Implementation(int32 AmmoToAdd)
{
	Ammo = FMath::Clamp(Ammo + AmmoToAdd, 0, MagCapacity);
	SetHUDAmmo();
}


void ARangeWeapon::AddAmmo(int32 AmmoToAdd)
{
	/*if (HasAuthority()) return;*/
	Ammo = FMath::Clamp(Ammo + AmmoToAdd, 0, MagCapacity);
	SetHUDAmmo();
	ClientAddAmmo(AmmoToAdd);
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




