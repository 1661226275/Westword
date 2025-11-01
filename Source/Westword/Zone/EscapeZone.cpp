// Fill out your copyright notice in the Description page of Project Settings.


#include "Zone/EscapeZone.h"
#include "Components/SphereComponent.h"
#include "Character/CowBoyCharacter.h"
#include "PlayerController/CowBoyPlayerController.h"

AEscapeZone::AEscapeZone()
{
 	
	PrimaryActorTick.bCanEverTick = false;
	ZoneSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ZoneSphere"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(StaticMeshComponent);
	ZoneSphere->SetupAttachment(StaticMeshComponent);

}


void AEscapeZone::BeginPlay()
{
	Super::BeginPlay();
	ZoneSphere->OnComponentBeginOverlap.AddDynamic(this, &AEscapeZone::OnSphereOverlap);
	ZoneSphere->OnComponentEndOverlap.AddDynamic(this, &AEscapeZone::OnSphereEndOverlap);
	OverlappingCharacter = nullptr;
}

void AEscapeZone::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACowBoyCharacter* Character = Cast<ACowBoyCharacter>(OtherActor);
	if (Character && !OverlappingCharacter)
	{
		OverlappingCharacter = Character;

		// 开始10秒撤离计时
		GetWorldTimerManager().SetTimer(LeaveTimerHandle, this, &AEscapeZone::OnEscapeTimerComplete, 10.0f, false);

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString("InEscape,10s"));
	}
}

void AEscapeZone::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 检查离开的是否是当前在区域内的角色
	ACowBoyCharacter* Character = Cast<ACowBoyCharacter>(OtherActor);
	if (Character && Character == OverlappingCharacter)
	{
		// 取消计时器
		GetWorldTimerManager().ClearTimer(LeaveTimerHandle);
		OverlappingCharacter = nullptr;

		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, FString("撤离取消"));
	}
}

void AEscapeZone::OnEscapeTimerComplete()
{
	if (OverlappingCharacter)
	{
		// 获取玩家的PlayerController
		ACowBoyPlayerController* PlayerController = Cast<ACowBoyPlayerController>(OverlappingCharacter->GetController());
		if (PlayerController)
		{
			PlayerController->SetGameEndHUD();
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString("撤离成功！"));
		}

		// 重置状态
		OverlappingCharacter = nullptr;
	}
}


