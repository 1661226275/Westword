// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CombatComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Character/CowBoyCharacter.h"
#include "Engine/SkeletalMeshSocket.h"


// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UCombatComponent::FireBottonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	ServerFire();
	
}
void UCombatComponent::ServerFire_Implementation()
{
	MultiCastFire();
}
void UCombatComponent::MultiCastFire_Implementation()
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
	
}



// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, Player_State);
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

