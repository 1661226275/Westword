// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/LagCompenstionComponent.h"
#include "Character/CowBoyCharacter.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"

ULagCompenstionComponent::ULagCompenstionComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;

	
}



void ULagCompenstionComponent::BeginPlay()
{
	Super::BeginPlay();


}

void ULagCompenstionComponent::SaveFramePackage(FFramPackage& Package)
{
	Character = Character == nullptr ? Cast<ACowBoyCharacter>(GetOwner()) : Character;
	if (Character)
	{
		Package.Time = GetWorld()->GetTimeSeconds();
		for (auto& BoxPair : Character->HitCollisionBoxes)
		{
			FBoxInformation BoxInformation;
			BoxInformation.Location = BoxPair.Value->GetComponentLocation();
			BoxInformation.Rotation = BoxPair.Value->GetComponentRotation();
			BoxInformation.BoxExtent = BoxPair.Value->GetScaledBoxExtent();
			Package.HitBoxInfo.Add(BoxPair.Key, BoxInformation);

		}
	}
}



void ULagCompenstionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (FrameHistory.Num() <= 1)
	{
		FFramPackage ThisFrame;
		SaveFramePackage(ThisFrame);
		FrameHistory.AddHead(ThisFrame);
	}
	else
	{
		float HistoryLength = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		while (HistoryLength > MaxRecordTime)
		{
			FrameHistory.RemoveNode(FrameHistory.GetTail());
			HistoryLength = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		}
		FFramPackage ThisFrame;
		SaveFramePackage(ThisFrame);
		FrameHistory.AddHead(ThisFrame); 
		ShowFramePackage(ThisFrame, FColor::Red);
	}
	
}

void ULagCompenstionComponent::ShowFramePackage(const FFramPackage& package, const FColor& Color)
{
	for (auto& BoxInfo : package.HitBoxInfo)
	{
		DrawDebugBox(
			GetWorld(),
			BoxInfo.Value.Location,
			BoxInfo.Value.BoxExtent,
			FQuat(BoxInfo.Value.Rotation),
			Color,
			false,
			4.f
		);
	}
}

