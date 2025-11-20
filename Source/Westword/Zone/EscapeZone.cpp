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

		// 获取PlayerController并显示倒计时UI
		ACowBoyPlayerController* PlayerController = Cast<ACowBoyPlayerController>(OverlappingCharacter->GetController());
		if (PlayerController)
		{
			PlayerController->ShowEscapeCountdown();
		}

		// 初始化剩余时间并开始重复计时器
		RemainingTime = 10.0f;
		GetWorldTimerManager().SetTimer(LeaveTimerHandle, this, &AEscapeZone::UpdateEscapeTimer, 1.0f, true);

		
	}
}

void AEscapeZone::UpdateEscapeTimer()
{
	if (OverlappingCharacter)
	{
		RemainingTime -= 1.0f;

		// 更新HUD显示
		ACowBoyPlayerController* PlayerController = Cast<ACowBoyPlayerController>(OverlappingCharacter->GetController());
		if (PlayerController)
		{
			PlayerController->SetHUDEscapeCountdown(RemainingTime);
		}

		// 检查是否计时结束
		if (RemainingTime <= 0.0f)
		{
			OnEscapeTimerComplete();
		}
	}
}

void AEscapeZone::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACowBoyCharacter* Character = Cast<ACowBoyCharacter>(OtherActor);
	if (Character && Character == OverlappingCharacter)
	{
		// 取消计时器
		GetWorldTimerManager().ClearTimer(LeaveTimerHandle);

		// 隐藏倒计时UI
		ACowBoyPlayerController* PlayerController = Cast<ACowBoyPlayerController>(OverlappingCharacter->GetController());
		if (PlayerController)
		{
			PlayerController->SetHUDEscapeCountdown(10.f);
			PlayerController->HideEscapeCountdown();
		}

		OverlappingCharacter = nullptr;

		
	}
}

void AEscapeZone::OnEscapeTimerComplete()
{
	if (OverlappingCharacter)
	{
		ACowBoyPlayerController* PlayerController = Cast<ACowBoyPlayerController>(OverlappingCharacter->GetController());
		if (PlayerController)
		{
			PlayerController->SetGameEndHUD();
			
			PlayerController->HideEscapeCountdown();
			
		}

		// 清理计时器
		GetWorldTimerManager().ClearTimer(LeaveTimerHandle);
		OverlappingCharacter = nullptr;
	}
}


