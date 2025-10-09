// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataType/EnumData.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"
#include "WeaponBase.generated.h"

UCLASS(Abstract, Blueprintable)
class WESTWORD_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();
	virtual void Tick(float DeltaTime) override;
	//拾取界面显示设置
	void SetPickUpWidgetVisibility(bool bIsVisible) { if (PickUpWeight) PickUpWeight->SetVisibility(bIsVisible); }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UStaticMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class USphereComponent* AreaSphere;

	UFUNCTION(BlueprintCallable)
	USphereComponent* GetAreaSphere() const { return AreaSphere; }

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickUpWeight;

	



public:	
    // 武器类型
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Properties")
    EWeaponType WeaponType;

	// 装备/卸下蒙太奇
	UPROPERTY(EditAnywhere, Category = "Animation")
	class UAnimMontage* EquipAnimMontage;

	UPROPERTY(EditAnywhere, Category = "Animation")
	class UAnimMontage* UnequipAnimMontage;
    // 播放装备武器蒙太奇
    UFUNCTION(BlueprintCallable)
    virtual void PlayEquipMontage();

    // 卸下武器  
    UFUNCTION(BlueprintCallable)
    virtual void PlayUnEquipMontage();

	UFUNCTION(BlueprintCallable)
	void SetWeaponState(EWeaponState NewState);

	UFUNCTION(BlueprintCallable)
	void GetWeaponState(EWeaponState& OutState) { OutState = WeaponState; }

	UStaticMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
};
