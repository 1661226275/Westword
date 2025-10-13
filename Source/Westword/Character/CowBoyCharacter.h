// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DataType/EnumData.h"
#include "Weapon/WeaponBase.h"
#include "Weapon/RangeWeapon.h"
#include "Components/CombatComponent.h"
#include "CowBoyCharacter.generated.h"

UCLASS()
class WESTWORD_API ACowBoyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACowBoyCharacter();
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void SetPlayingMantogeState(EPlayingMantoge NewState) { PlayingMantogeState = NewState; }
	UFUNCTION(BlueprintCallable)
	void SetWeaponType(EWeaponType NewType) { WeaponType = NewType; }

	//网络复制函数
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//重叠武器设置函数,这个函数只会在服务器上被调用
	void SetOverLapWeapon(AWeaponBase* Weapon);

	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintCallable)
	UCombatComponent* GetCombatComponent() const { return Combat; }

	float GetForwardInput() const { return ForwardInput; }
	void SetForWardInput(float num) { ForwardInput = num; }
	float GetRightInput() const { return RightInput; }
	void SetRightInput(float num) { RightInput = num; }
	UFUNCTION(BlueprintCallable)
	AWeaponBase* GetWeapon(int index) const { return WeaponSolts[index]; }

	bool IsAiming() { return Combat && Combat->Player_State == ECharacterState::CharacterState_Aim; }

	void AimOffset(float DeltaTime);

	float GetAOPitch() const { return AO_Pitch; }

	FVector GetHitTarget()const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void Turn(float Value);

	void LookUp(float Value);
	
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


	void PickUpBottonPressed();

	void AimBottonPressed();
	void AimBottonReleased();

	void FireBottonPressed();
	void FireBottonReleased();

private:
	UPROPERTY(VisibleAnyWhere, Category = Camera)
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnyWhere, Category= Camera)
	class UCameraComponent* ThirdViewCamera;

	UPROPERTY(EditAnywhere, Category = Animation)
	class UAnimMontage* SlideMontage;

	//武器槽,2个武器槽，近战和远程
	UPROPERTY( VisibleAnyWhere, Category = Weapon)
	TArray<AWeaponBase*>WeaponSolts;

	UPROPERTY( EditAnywhere, Category = Weapon)
	TSubclassOf<AWeaponBase> WeaponClass;

	//网络复制变量
	UPROPERTY(ReplicatedUsing = RepNotify_OverLapWeapon)
	class AWeaponBase* OverLapWeapon;

	//网络复制通知
	UFUNCTION()
	void RepNotify_OverLapWeapon(AWeaponBase* LastWeapon);

	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;

	float ForwardInput;
	float RightInput;

	float AO_Pitch;


public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
	EPlayingMantoge PlayingMantogeState = EPlayingMantoge::PlayingMantoge_Blank;


	UPROPERTY( BlueprintReadWrite, Category = "PlayerState")
	EWeaponType WeaponType = EWeaponType::WeaponType_None;



	
};


