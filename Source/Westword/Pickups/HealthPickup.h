// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/Pickup.h"
#include "HealthPickup.generated.h"

/**
 * 
 */
UCLASS()
class WESTWORD_API AHealthPickup : public APickup
{
	GENERATED_BODY()

public:
	
	virtual void Interact(APawn* User) override;
	virtual void InteractEffect(APawn* User) override;
protected:

private:
	UPROPERTY(EditAnywhere)
	float HealthAmount = 50.0f;
	UPROPERTY(EditAnywhere)
	float HealingTime = 5.f;



	
};
