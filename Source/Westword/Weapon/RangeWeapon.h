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

	UPROPERTY(EditAnyWhere)
	TSubclassOf<AProjectile> ServerSideRewindProjectileClass;


	/*
	* Zoomed FOV while aiming
	*/
	UPROPERTY(EditAnywhere)
	float ZoomFov = 30.f;

	UPROPERTY(EditAnywhere)
	float ZoomInterSpeed = 20.f;

	UPROPERTY(EditAnywhere)
	int32 Ammo;

	UFUNCTION(Client,Reliable)
	void ClientUpdateAmmo(int32 ServerAmmo);
	UFUNCTION(Client, Reliable)
	void ClientAddAmmo(int32 AmmoToAdd);

	void SpendRound();



	UPROPERTY(EditAnywhere)
	int32 MagCapacity;

	//未处理的server requests数量
	//客户端开火后进行增加，收到回复后减少
	int32 Sequence;

	virtual void OnRep_Owner() override;

	


public:

	float GetProjectileDamage() const;
	float GetProjectileHeadShotDamage() const;
	// 开火蒙太奇
	UPROPERTY(EditAnywhere)
	class UAnimMontage* FireMontage;
	//换弹蒙太奇
	UPROPERTY(EditAnywhere)
	class UAnimMontage* ReloadMontage;
	// 开火
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void PlayFireMontage(bool bIsAiming);
	void Fire(const FVector& HitTarget);
	// 换弹
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void PlayReloadMontage();

    //装备武器
	virtual void PlayEquipMontage() override;

	//卸下武器
	virtual void PlayUnEquipMontage() override;

	virtual EWeaponType GetWeaponType() override { return WeaponType; }

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
	virtual void SetWeaponHUDVisible(bool bIsVisible) override;

	virtual bool CanAttack() override { return Ammo > 0; }

	int32 GetAmmo() const { return Ammo; }
	int32 GetMagCapacity() const { return MagCapacity; }

	void AddAmmo(int32 AmmoToAdd);


};
