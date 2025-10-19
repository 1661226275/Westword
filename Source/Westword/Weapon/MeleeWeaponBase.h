
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

protected:
	UPROPERTY(EditAnywhere, Category = "Melee")
	float Damage = 50;

	//¹¥»÷¼ì²â
	void MeleeHitChannelTrace();

private:
	//¹¥»÷ÃÉÌ«Ææ
	UPROPERTY(EditAnywhere)
	class UAnimMontage* AttackMontage;
	UPROPERTY(EditAnyWhere)
	class USoundCue* ImpactSurfaceSound;
	UPROPERTY(EditAnyWhere)
	class USoundCue* ImpactBodySound;

	
	//×°±¸ÎäÆ÷
	virtual void PlayEquipMontage() override;
	//Ð¶ÏÂÎäÆ÷
	virtual void PlayUnEquipMontage() override;
	virtual void OnRep_Owner() override;
	
	
	

	/*
	* param for Sphere Channel Trace
	*/
	UPROPERTY(EditAnywhere)
	float DamageTraceRange = 50.f;
	class AActor* LastHitActor;

	FTimerHandle AttackTimeHandle;
	
};
