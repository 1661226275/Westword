// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapon/WeaponBase.h"
#include "Weapon/RangeWeapon.h"
#include "Weapon/MeleeWeaponBase.h"
#include "Westword/HUD/CowBoyHUD.h"
#include "DataType/EnumData.h"
#include "CombatComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
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


	void Reload();
	UFUNCTION(Server, Reliable)
	void ServerReload();
	//¹ã²¥»»µ¯¶¯»­
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastReload();

	int AmountToReload();
	
	UFUNCTION()
	void OnPingTooHigh(bool bPingTooHigh);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void AttackBottonPressed(bool bPressed);

	bool bFireButtonPressed = false;
	void LocalAttack(const FVector_NetQuantize& TraceHitTarget);
	UFUNCTION(Server, Reliable)
	void ServerAttack(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastAttack(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);
private:
	class ACowBoyCharacter* Character;
	UPROPERTY(Replicated)
	class AWeaponBase* EquippedWeapon;

	class ACowBoyPlayerController* Controller;
	class ACowBoyHUD* HUD;

	/*
	CrossHairs and HUD
	*/
	float CrosshairVelocityFactor;

	float CrosshairInAirFactor;

	float CrosshairAimFactor;

	float CrosshairShootingFactor;

	FVector HitTarget;

	FHUDPackage HUDPackage;

	/*
	Aiming and Fov
	*/
	float DefaultFOV;
	float CurrentFOV;
	UPROPERTY(EditAnywhere,Category = "Combat")
	float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomInterSpeed = 20.f;

	void InterFov(float DeltaTime);


	//Carried Ammo for the currently equipped weapon
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	TMap<EWeaponType, int32> CarriedAmmoMap;

	UPROPERTY(EditAnywhere, Category = "Combat")
	int32 StatrtingRoundAmmo = 30;
	void InitializeCarriedAmmo();

	void UpdateAmmoValues();

	UFUNCTION(BlueprintCallable)
	void FinishReloading();

public:
	UFUNCTION(BlueprintCallable)
	void EquipWeapon(AWeaponBase* WeaponToEquip);
	UFUNCTION(BlueprintCallable)
	void UnEquipWeapon();

	

	UPROPERTY(ReplicatedUsing = OnRep_PlayerStateChange, VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
	ECharacterState Player_State = ECharacterState::CharacterState_Norm;


	bool bAimButtonPressed = false;
	UFUNCTION()
	void OnRep_PlayerStateChange();

	FVector GetHitTarget() const {return HitTarget;}
	UFUNCTION(BlueprintCallable)
	AWeaponBase* GetEquippedWeapon() const { return EquippedWeapon; }

	bool CanAttack();

	bool IsAlive() const { return Player_State == ECharacterState::CharacterState_Death; }
};
