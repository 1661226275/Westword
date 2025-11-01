// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EscapeZone.generated.h"

UCLASS()
class WESTWORD_API AEscapeZone : public AActor
{
	GENERATED_BODY()
	
public:	
	AEscapeZone();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

private:
	UPROPERTY(EditAnywhere)
	class USphereComponent* ZoneSphere;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* StaticMeshComponent;

	FTimerHandle LeaveTimerHandle;

	// ��ʱ����ɻص�����
	UFUNCTION()
	void OnEscapeTimerComplete();

	// ��ǰ�������ڵĽ�ɫ
	class ACowBoyCharacter* OverlappingCharacter;


public:	
	

};
