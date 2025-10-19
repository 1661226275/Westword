
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponBase.h"
#include "DataType/EnumData.h"
#include "MeleeWeaponBase.generated.h"

/**
 * 
 */
UCLASS()
class WESTWORD_API AMeleeWeaponBase : public AWeaponBase
{
	GENERATED_BODY()

public:

	void PlayAttackMontage();
	void HandleAttack();

	UFUNCTION(BlueprintCallable)
	void EndAttack();

	virtual EWeaponType GetWeaponType() override { return WeaponType; }

protected:
	UPROPERTY(EditAnywhere, Category = "Melee")
	float Damage = 30;

	//�������
	void MeleeHitChannelTrace();

private:
	//������̫��
	UPROPERTY(EditAnywhere)
	class UAnimMontage* AttackMontage;

	
	//װ������
	virtual void PlayEquipMontage() override;
	//ж������
	virtual void PlayUnEquipMontage() override;
	virtual void OnRep_Owner() override;

	
	

	/*
	* param for Sphere Channel Trace
	*/
	UPROPERTY(EditAnywhere)
	float DamageTraceRange = 30.f;
	class AActor* LastHitActor;

	FTimerHandle AttackTimeHandle;
	
};
