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

	/*
	* Zoomed FOV while aiming
	*/
	UPROPERTY(EditAnywhere)
	float ZoomFov = 30.f;

	UPROPERTY(EditAnywhere)
	float ZoomInterSpeed = 20.f;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Ammo)
	int32 Ammo;

	void SpendRound();

	UFUNCTION()
	void OnRep_Ammo();

	UPROPERTY(EditAnywhere)
	int32 MagCapacity;

	virtual void OnRep_Owner() override;

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

	UPROPERTY(EditAnywhere, Category = "CrossHair")
	class UTexture2D* CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = "CrossHair")
	class UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = "CrossHair")
	class UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = "CrossHair")
	class UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = "CrossHair")
	class UTexture2D* CrosshairsBottom;

	float GetZoomedFov() const { return ZoomFov; }
	float GetZoomInterSpeed() const { return ZoomInterSpeed; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void SetHUDAmmo();

	bool CanFire() { return Ammo > 0; }
};
