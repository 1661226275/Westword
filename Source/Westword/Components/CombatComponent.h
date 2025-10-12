// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapon/WeaponBase.h"
#include "Weapon/RangeWeapon.h"
#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WESTWORD_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	friend class ACowBoyCharacter;
	
	void SetPlayerState(ECharacterState NewState);
	UFUNCTION(Reliable, Server)
	void ServerSetPlayerState(ECharacterState NewState);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void FireBottonPressed(bool bPressed);

	bool bFireButtonPressed = false;

	UFUNCTION(Server,Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast,Reliable)
	void MultiCastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

private:
	class ACowBoyCharacter* Character;
	UPROPERTY(Replicated)
	class AWeaponBase* EquippedWeapon;


public:	
	UFUNCTION(BlueprintCallable)
	void EquipWeapon(AWeaponBase* WeaponToEquip);

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
	ECharacterState Player_State = ECharacterState::CharacterState_Norm;
		
};
