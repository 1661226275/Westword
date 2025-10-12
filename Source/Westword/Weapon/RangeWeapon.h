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

	// ¿ª»ðÃÉÌ«Ææ
	UPROPERTY(EditAnywhere)
	class UAnimMontage* FireMontage;
	//»»µ¯ÃÉÌ«Ææ
	UPROPERTY(EditAnywhere)
	class UAnimMontage* ReloadMontage;
	// ¿ª»ð
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void PlayFireMontage(bool bIsAiming);
	void Fire(const FVector& HitTarget);
	// »»µ¯
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void PlayReloadMontage();

    //×°±¸ÎäÆ÷
	virtual void PlayEquipMontage() override;

	//Ð¶ÏÂÎäÆ÷
	virtual void PlayUnEquipMontage() override;
	
};
