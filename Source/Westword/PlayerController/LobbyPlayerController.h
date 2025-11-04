// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Weapon/WeaponDisplayActor.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class WESTWORD_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()


public:
	void LobbyMenuSetUp();

	virtual void BeginPlay() override;

    // 创建本地武器展示
    UFUNCTION(Client, Reliable)
    void ClientCreateLocalWeaponDisplay();

    // 更新本地武器展示
    UFUNCTION(Client, Reliable)
    void ClientUpdateWeaponDisplay(bool bIsRoundWeapon);




private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget>LobbyWidgetClass;
	
	UPROPERTY()
	class ULobbyWidget* LobbyWidget;

    // 武器展示Actor类引用
    UPROPERTY(EditAnywhere, Category = "Weapon Display")
    TSubclassOf<AWeaponDisplayActor> WeaponDisplayActorClass;

    // 本地武器展示Actor
    UPROPERTY()
    AWeaponDisplayActor* LocalWeaponDisplayActor;

    // 武器展示位置偏移
    UPROPERTY(EditAnywhere, Category = "Weapon Display")
    FVector WeaponDisplayOffset = FVector(256.0f, -958.0f, 156.0f);
    // 武器展示旋转
    UPROPERTY(EditAnywhere, Category = "Weapon Display")
    FRotator WeaponDisplaRotate = FRotator(0.f, 0.f, 0.f);
    
};
