// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponBase.h"
#include "RangeWeapon.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class WESTWORD_API ARangeWeapon : public AWeaponBase
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnyWhere)
	TSubclassOf<class AProjectile> ProjectileClass;

public:

	// ������̫��
	UPROPERTY(EditAnywhere)
	class UAnimMontage* FireMontage;
	//������̫��
	UPROPERTY(EditAnywhere)
	class UAnimMontage* ReloadMontage;
	// ����
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void PlayFireMontage(bool bIsAiming);
	void Fire(const FVector& HitTarget);
	// ����
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void PlayReloadMontage();

    //װ������
	virtual void PlayEquipMontage() override;

	//ж������
	virtual void PlayUnEquipMontage() override;
	
};
