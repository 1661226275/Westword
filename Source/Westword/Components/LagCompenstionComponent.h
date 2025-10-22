// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LagCompenstionComponent.generated.h"


USTRUCT(BlueprintType)
struct FBoxInformation
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FRotator Rotation;

	UPROPERTY()
	FVector BoxExtent;
};

USTRUCT(BlueprintType)
struct FFramPackage
{
	GENERATED_BODY()

	UPROPERTY()
	float Time;

	UPROPERTY()
	TMap<FName, FBoxInformation>HitBoxInfo;

};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WESTWORD_API ULagCompenstionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	ULagCompenstionComponent();
	friend class ACowBoyCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ShowFramePackage(const FFramPackage& package, const FColor& Color);
protected:

	virtual void BeginPlay() override;
	void SaveFramePackage(FFramPackage& Package);
private:
	UPROPERTY()
	class ACowBoyCharacter* Character;

	UPROPERTY()
	class ACowBoyPlayerController* Controller;

	TDoubleLinkedList<FFramPackage>FrameHistory;
	
	UPROPERTY(EditAnywhere)
	float MaxRecordTime = 4.f;

public:	


		
};
