// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/LobbyPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "Blueprint/UserWidget.h"
#include "HUD/LobbyWidget.h"


void ALobbyPlayerController::BeginPlay()
{
    Super::BeginPlay();
    if (IsLocalController())
    {
        LobbyMenuSetUp();
    }
    
}




void ALobbyPlayerController::LobbyMenuSetUp()
{
    // 1. 查找并锁定到Camera Actor
    TArray<AActor*> FoundCameras;
    UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ACameraActor::StaticClass(), FName("CharacterCamera"), FoundCameras);
    if (FoundCameras.Num() > 0)
    {
        ACameraActor* LobbyCamera = Cast<ACameraActor>(FoundCameras[0]);
        if (LobbyCamera)
        {
            SetViewTarget(LobbyCamera);
            SetInputMode(FInputModeUIOnly());
            bShowMouseCursor = true;
        }
    }

    // 2. 创建用户界面
    if (LobbyWidgetClass)
    {
        LobbyWidget = CreateWidget<ULobbyWidget>(this, LobbyWidgetClass);
        if (LobbyWidget)
        {
            LobbyWidget->AddToViewport();
            LobbyWidget->SetUp();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("LobbyWidgetClass is not set in LobbyPlayerController"));
    }
    // 3. 创建本地武器展示
    ClientCreateLocalWeaponDisplay();
    UE_LOG(LogTemp, Log, TEXT("Lobby setup completed for player: %s"), *GetName());
}

void ALobbyPlayerController::ClientCreateLocalWeaponDisplay_Implementation()
{
    // 只在本地客户端创建武器展示Actor
    if (WeaponDisplayActorClass && IsLocalController())
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        LocalWeaponDisplayActor = GetWorld()->SpawnActor<AWeaponDisplayActor>(
            WeaponDisplayActorClass,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            SpawnParams
        );

        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("WeaponActor"), FoundActors);
        if (FoundActors.Num() > 0)
        {
            AActor* ReferenceActor = FoundActors[0];
            LocalWeaponDisplayActor->InitializeFromReferenceActor(ReferenceActor);
            UE_LOG(LogTemp, Log, TEXT("Weapon display initialized from reference actor"));
        }
    }
}

void ALobbyPlayerController::ClientUpdateWeaponDisplay_Implementation(bool bIsRoundWeapon)
{
    // 这个函数在本地客户端执行（无论是监听服务器还是远程客户端）
    if (LocalWeaponDisplayActor)
    {
        LocalWeaponDisplayActor->UpdateWeaponMesh(bIsRoundWeapon);
        UE_LOG(LogTemp, Log, TEXT("Weapon display updated for player: %s"), *GetName());
    }
}