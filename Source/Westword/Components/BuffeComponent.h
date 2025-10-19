// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WESTWORD_API UBuffeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBuffeComponent();
	friend class ACowBoyCharacter;
	void Heal(float HealAmount, float HealingTime);
	void BuffSpeed(float BuffBaseSpeed,float BuffTime);
	void BuffJump(float BuffJumpZVelocity, float BuffTime);
	void SetInitialBaseSpeed(float Speed) { IntialBaseSpeed = Speed; }
	void SetInitialJumpZVelocity(float JumpZVelocity) { IntialJumpZVelocity = JumpZVelocity; }
private:
	UPROPERTY()
	class ACowBoyCharacter* Character;

	/*
	* Heal buff
	*/
	bool bHealing = false;
	float HealRate = 0.f;
	float AmountToHeal = 0.f;

	/*
	* speed buff
	*/
	FTimerHandle SpeedBuffTimer;
	void ResetSpeed();
	float IntialBaseSpeed;

	/*
	* Jump buff
	*/
	
	FTimerHandle JumpBuffTimer;
	void ResetJump();
	float IntialJumpZVelocity;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void HealRampUp(float DeltaTime);
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
