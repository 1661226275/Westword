// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "Pickup.generated.h"

UCLASS()
class WESTWORD_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	
	APickup();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
	void SetInteractWidgetVisibility(bool bIsVisible);
	virtual void Interact(APawn* User);
	UFUNCTION(BlueprintCallable)
	virtual void InteractEffect(APawn* User);

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
	virtual void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, Category = "InteractorAnim")
	class UAnimMontage*InteractorAnimation;

private:
	UPROPERTY(EditAnywhere)
	class USphereComponent* OverlapSphere;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* PickupMesh;
	UPROPERTY(EditAnywhere)
	class USoundCue* PickupSound;
	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* InteractWidget;

public:	
	

};
