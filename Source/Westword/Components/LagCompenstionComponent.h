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

USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	bool bHitConfirmed;

	UPROPERTY()
	bool bHeadShot;
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
	FServerSideRewindResult ServerSideRewind(
		class ACowBoyCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation,
		float HitTime);
	UFUNCTION(Server,Reliable)
	void ServerScoreRequest(
		ACowBoyCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation,
		float HitTime,
		class AWeaponBase* DamageCauser
	);
	/*
	* Projectile
	*/
	FServerSideRewindResult ProjectileServerSideRewind(
		class ACowBoyCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime
	);

	UFUNCTION(Server,Reliable)
	void ProjectileServerScoreRequest(
		ACowBoyCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime
	);

protected:

	virtual void BeginPlay() override;
	void SaveFramePackage(FFramPackage& Package);
	
	FFramPackage InterpBetweenFrames(const FFramPackage& OlderFrame, const FFramPackage& YoungerFrame, float HitTime);
	
	void CacheBoxPosition(ACowBoyCharacter* HitCharacter, FFramPackage& OutFramePackage);
	void MoveBoxes(ACowBoyCharacter* HitCharacter, const FFramPackage& FramePackage);
	void ResetHitBoxes(ACowBoyCharacter* HitCharacter, const FFramPackage& FramePackage);
	void EnableCharacterMeshCollision(ACowBoyCharacter* CowBoyCharacter, ECollisionEnabled::Type CollisionEnableType);
	void SaveFramePackage();

	/*
	* HitScan
	*/
	FServerSideRewindResult ConfirmHit(
		const FFramPackage& Package,
		class ACowBoyCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation);

	/*
	* Projectile
	*/
	FServerSideRewindResult ProjectileConfirmHit(
		const FFramPackage& Package,
		class ACowBoyCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime
	);
private:
	UPROPERTY()
	class ACowBoyCharacter* Character;

	UPROPERTY()
	class ACowBoyPlayerController* Controller;

	TDoubleLinkedList<FFramPackage>FrameHistory;
	
	UPROPERTY(EditAnywhere)
	float MaxRecordTime = 4.f;

	FFramPackage GetFrameToCheck(ACowBoyCharacter* HitCharacter, float HitTime);

public:	


		
};
