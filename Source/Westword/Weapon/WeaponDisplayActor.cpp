// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponDisplayActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"


AWeaponDisplayActor::AWeaponDisplayActor()
{
 
	PrimaryActorTick.bCanEverTick = false;
    bReplicates = false;
    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    // 创建左轮网格组件
    RoundWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RoundWeaponMesh"));
    RoundWeaponMesh->SetupAttachment(RootComponent);
    
    RoundWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    // 创建Rlder网格组件
    RlderWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RlderWeaponMesh"));
    RlderWeaponMesh->SetupAttachment(RootComponent);
    
    RlderWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void AWeaponDisplayActor::BeginPlay()
{
	Super::BeginPlay();
    if (DefaultRoundMesh)
    {
        RoundWeaponMesh->SetStaticMesh(DefaultRoundMesh);
        RoundWeaponMesh->SetVisibility(false);
    }

    if (DefaultRlderMesh)
    {
        RlderWeaponMesh->SetStaticMesh(DefaultRlderMesh);
        RlderWeaponMesh->SetVisibility(false);
    }
}

void AWeaponDisplayActor::UpdateWeaponMesh(bool bIsRoundWeapon)
{
    if (bIsRoundWeapon)
    {
        RlderWeaponMesh->SetVisibility(false);
        RoundWeaponMesh->SetVisibility(true);
    }
    else
    {
        RlderWeaponMesh->SetVisibility(true);
        RoundWeaponMesh->SetVisibility(false);
    }
}

void AWeaponDisplayActor::SetDisplayOffset(const FVector& Offset)
{
    SetActorRelativeLocation(Offset);
}

void AWeaponDisplayActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponDisplayActor::InitializeFromReferenceActor(AActor* ReferenceActor)
{
    if (!ReferenceActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("ReferenceActor is null!"));
        return;
    }

    // 1. 复制Actor的变换
    SetActorTransform(ReferenceActor->GetActorTransform());

    // 2. 设置缩放
    SetActorScale3D(FVector(1.3f, 1.3f, 1.3f));

    // 3. 查找并复制网格组件设置
    TArray<UStaticMeshComponent*> MeshComponents;
    ReferenceActor->GetComponents<UStaticMeshComponent>(MeshComponents);

    for (UStaticMeshComponent* RefMeshComp : MeshComponents)
    {
        if (RefMeshComp->ComponentHasTag(FName("RoundWeaponMesh")))
        {
            // 复制远程武器网格设置
            RoundWeaponMesh->SetRelativeTransform(RefMeshComp->GetRelativeTransform());
            UE_LOG(LogTemp, Log, TEXT("Found and initialized RoundWeaponmesh from reference"));
        }
        else if (RefMeshComp->ComponentHasTag(FName("RiderMesh")))
        {
            // 复制近战武器网格设置
            RlderWeaponMesh->SetRelativeTransform(RefMeshComp->GetRelativeTransform());
            UE_LOG(LogTemp, Log, TEXT("Found and initialized RlderWeaponMesh from reference"));
        }
    }

    UE_LOG(LogTemp, Log, TEXT("WeaponDisplayActor initialized from reference actor: %s"), *ReferenceActor->GetName());
}

