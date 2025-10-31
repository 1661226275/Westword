// Fill out your copyright notice in the Description page of Project Settings.


#include "Zone/EscapeZone.h"
#include "Components/SphereComponent.h"
#include "Character/CowBoyCharacter.h"

AEscapeZone::AEscapeZone()
{
 	
	PrimaryActorTick.bCanEverTick = false;
	ZoneSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ZoneSphere"));
	SetRootComponent(ZoneSphere);

}


void AEscapeZone::BeginPlay()
{
	Super::BeginPlay();
	ZoneSphere->OnComponentBeginOverlap.AddDynamic(this, &AEscapeZone::OnSphereOverlap);
}

void AEscapeZone::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACowBoyCharacter* OverlappingCharacter = Cast<ACowBoyCharacter>(OtherActor);
	if (OverlappingCharacter)
	{
		//¿ªÊ¼³·Àë
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString("InEscapeZonz"));
	}
}


