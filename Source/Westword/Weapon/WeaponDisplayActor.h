// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponDisplayActor.generated.h"

UCLASS()
class WESTWORD_API AWeaponDisplayActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponDisplayActor();
	virtual void Tick(float DeltaTime) override;

	// 从参考Actor初始化
	UFUNCTION(BlueprintCallable)
	void InitializeFromReferenceActor(AActor* ReferenceActor);

protected:
	virtual void BeginPlay() override;

public:	
	// 更新武器网格
	UFUNCTION(BlueprintCallable)
	void UpdateWeaponMesh(bool bIsRoundWeapon);

	// 设置武器展示位置偏移
	UFUNCTION(BlueprintCallable)
	void SetDisplayOffset(const FVector& Offset);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    class UStaticMeshComponent* RoundWeaponMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    class UStaticMeshComponent* RlderWeaponMesh;

    // 默认远程武器网格
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UStaticMesh* DefaultRoundMesh;

    // 默认近战武器网格
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UStaticMesh* DefaultRlderMesh;

};
