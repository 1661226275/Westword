// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/Pickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Character/CowBoyCharacter.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"


APickup::APickup()
{
 	
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(RootComponent);
	OverlapSphere->SetSphereRadius(150.f);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	PickupMesh->SetupAttachment(OverlapSphere);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	InteractWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget"));
	InteractWidget->SetupAttachment(PickupMesh);
}


void APickup::BeginPlay()
{
	Super::BeginPlay();
	if(HasAuthority())
	{
		if(OverlapSphere)
		{
			OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &APickup::OnSphereOverlap);
			OverlapSphere->OnComponentEndOverlap.AddDynamic(this, &APickup::OnSphereEndOverlap);
		}
		
	}
	InteractWidget->SetVisibility(false);
	
}

void APickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACowBoyCharacter* CowBoyCharacter = Cast<ACowBoyCharacter>(OtherActor);
	if(CowBoyCharacter)
	{
		CowBoyCharacter->SetOverLapInteractActor(this);
	}
}
void APickup::OnSphereEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACowBoyCharacter* CowBoyCharacter = Cast<ACowBoyCharacter>(OtherActor);
	if (CowBoyCharacter)
	{
		CowBoyCharacter->SetOverLapInteractActor(nullptr);
	}
}


void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickup::Destroyed()
{
	Super::Destroyed();
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
	}
}

void APickup::SetInteractWidgetVisibility(bool bIsVisible)
{
	if (InteractWidget)
	{
		InteractWidget->SetVisibility(bIsVisible);
	}
}

void APickup::Interact(APawn* User)
{
}

void APickup::InteractEffect(APawn* User)
{
}

