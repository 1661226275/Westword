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

		// ��ʼ10�볷���ʱ
		GetWorldTimerManager().SetTimer(LeaveTimerHandle, this, &AEscapeZone::OnEscapeTimerComplete, 10.0f, false);

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString("InEscape,10s"));
	}
}

void AEscapeZone::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// ����뿪���Ƿ��ǵ�ǰ�������ڵĽ�ɫ
	ACowBoyCharacter* Character = Cast<ACowBoyCharacter>(OtherActor);
	if (Character && Character == OverlappingCharacter)
	{
		// ȡ����ʱ��
		GetWorldTimerManager().ClearTimer(LeaveTimerHandle);
		OverlappingCharacter = nullptr;

		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, FString("����ȡ��"));
	}
}

void AEscapeZone::OnEscapeTimerComplete()
{
	if (OverlappingCharacter)
	{
		// ��ȡ��ҵ�PlayerController
		ACowBoyPlayerController* PlayerController = Cast<ACowBoyPlayerController>(OverlappingCharacter->GetController());
		if (PlayerController)
		{
			PlayerController->SetGameEndHUD();
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString("����ɹ���"));
		}

		// ����״̬
		OverlappingCharacter = nullptr;
	}
}


