// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DataType/EnumData.h"
#include "Weapon/WeaponBase.h"
#include "Weapon/RangeWeapon.h"
#include "Weapon/MeleeWeaponBase.h"
#include "Interfaces/InteractWithCrosshairsInterface.h"
#include "Components/CombatComponent.h"
#include "Components/BuffeComponent.h"
#include "CowBoyCharacter.generated.h"

UCLASS(Blueprintable, BlueprintType)
class WESTWORD_API ACowBoyCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	ACowBoyCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintCallable)
	void SetPlayingMantogeState(EPlayingMantoge NewState) { PlayingMantogeState = NewState; }
	UFUNCTION(BlueprintCallable)
	void SetWeaponType(EWeaponType NewType) { WeaponType = NewType; }
	void SetForWardInput(float num) { ForwardInput = num; }
	void SetRightInput(float num) { RightInput = num; }

	//重叠武器设置函数,这个函数只会在服务器上被调用
	void SetOverLapWeapon(AWeaponBase* Weapon);
	void SetOverLapInteractActor(class APickup* Actor);

	

	

	float GetForwardInput() const { return ForwardInput; }
	float GetRightInput() const { return RightInput; }
	float GetAOPitch() const { return AO_Pitch; }
	UFUNCTION(BlueprintCallable)
	UCombatComponent* GetCombatComponent() const { return Combat; }
	UFUNCTION(BlueprintCallable)
	AWeaponBase* GetWeapon(int index) const { return WeaponSolts[index]; }
	bool IsAiming() { return Combat && Combat->Player_State == ECharacterState::CharacterState_Aim; }
	void AimOffset(float DeltaTime);

	

	void Elim();
	UFUNCTION(NetMulticast, Reliable)
	void MultCastElim();
	//复活请求
	void RequestRespawn();
	//复活
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRespawn();
	void UpdateHUDHealth();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void ActivateSkill1();
	void ActivateSkill2();
	virtual void Jump() override;
	void Slide();
	UFUNCTION(Server, Reliable)
	void ServerSlide();
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastSlide();
	void StartSprint();
	void EndSprint();
	void EquipRangeWeaponBottonPressed();
	UFUNCTION(Server, Reliable)
	void ServerEquipRangeWeapon();
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastEquipRangeWeapon();

	void EquipMeleeWeaponBottonPressed();
	UFUNCTION(Server, Reliable)
	void ServerEquipMeleeWeapon();
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastEquipMeleeWeapon();


	void PickUpBottonPressed();
	void AimBottonPressed();
	void AimBottonReleased();
	void AttackBottonPressed();
	void AttackBottonReleased();
	void ReloadBottonPressed();
	void PlayHitReactMontage();

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	//Poll for any relevant classes and initialize our HUD
	void PollInit();

private:
	UPROPERTY(VisibleAnyWhere, Category = Camera)
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnyWhere, Category= Camera)
	class UCameraComponent* ThirdViewCamera;

	UPROPERTY(EditAnywhere, Category = Animation)
	class UAnimMontage* SlideMontage;

	UPROPERTY(EditAnywhere, Category = Animation)
	class UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Animation)
	class UAnimMontage* DieMontage;

	UPROPERTY(EditAnywhere, Category = Animation)
	class UAnimMontage* RespawnMontage;


	

	//武器槽,2个武器槽，近战和远程
	UPROPERTY(Replicated,VisibleAnyWhere, Category = Weapon)
	TArray<AWeaponBase*>WeaponSolts;
	UPROPERTY( EditAnywhere, Category = Weapon)
	TArray<TSubclassOf<AWeaponBase>> WeaponClass;

	//网络复制变量
	UPROPERTY(ReplicatedUsing = RepNotify_OverLapWeapon)
	class AWeaponBase* OverLapWeapon;

	UPROPERTY(ReplicatedUsing = RepNotify_OverLapInteractActor)
	class APickup* OverLapInteractActor;

	//网络复制通知
	UFUNCTION()
	void RepNotify_OverLapWeapon(AWeaponBase* LastWeapon);

	UFUNCTION()
	void RepNotify_OverLapInteractActor(class APickup* LastActor);

	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;

	UPROPERTY(VisibleAnywhere)
	class UBuffeComponent* Buff;
	float ForwardInput;
	float RightInput;

	float AO_Pitch;

	void HideCharacterIfCharacterClose();
	UPROPERTY(EditAnywhere, Category = Camera)
	float CameraThreshold = 200.f;

	/*
	player Health
	*/

	UPROPERTY(EditAnywhere,Category = "Player State")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player State")
	float Health = 100.f;

	UFUNCTION()
	void OnRep_Health(float LastHealth);

	TMap<int32, FName> EquipWeaponSocket;

	class ACowBoyPlayerController* CowBoyController;

	/*
	* player movement
	*/
	UPROPERTY(EditAnyWhere)
	float MaxWalkSpeed = 200.f;
	UPROPERTY(EditAnyWhere)
	float MaxSprintSpeed = 500.f;

	bool bElimmed = false;
	bool bShootHead = false;
	bool bIsSprinting = false;

public:	

	

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
	EPlayingMantoge PlayingMantogeState = EPlayingMantoge::PlayingMantoge_Blank;


	UPROPERTY( BlueprintReadWrite, Category = "PlayerState")
	EWeaponType WeaponType = EWeaponType::WeaponType_None;

	UPROPERTY(BlueprintReadWrite, Category = "PlayerState")
	EDeathpose Deathpose;

	UFUNCTION(BlueprintCallable)
	void SetDeathPose(EDeathpose NewPose) { Deathpose = NewPose; }

	UCameraComponent* GetThirdViewCamera() const { return ThirdViewCamera; }

	bool IsElimmed() const { return bElimmed; }
	bool IsShootHead() const { return bShootHead; }

	void PlayDieMontage();

	void PlayDieShootHeadMontage();

	float GetHealth() const { return Health; }
	float GetMaxHealth() const { return MaxHealth; }
	void SetHealth(float NewHealth) { Health = NewHealth; }
	void SetMaxWalkSpeed(float NewSpeed) { MaxWalkSpeed = NewSpeed; }
	void SetMaxSprintSpeed(float NewSpeed) { MaxSprintSpeed = NewSpeed; }
	bool IsSprinting() const {return bIsSprinting;}
	class ACowBoyPlayerState* CowBoyPlayerState;
	UBuffeComponent* GetBuffComponent() const { return Buff; }
};


