// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActivateSkillBase.generated.h"

UCLASS()
class WESTWORD_API AActivateSkillBase : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AActivateSkillBase();
	virtual void Tick(float DeltaTime) override;
	virtual void ActivateSkill(class APawn* Character);
	virtual void SkillEffectFunction(class APawn* Character);
	virtual void DeactivateSkill(class APawn* Character);
protected:
	
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere)
	float SkillCooldown = 20.f;
	UPROPERTY(EditAnywhere)
	float SkillDuration = 10.f;
	UPROPERTY(EditAnywhere)
	float SanCost = 30.f;

	UPROPERTY(EditAnywhere)
	class USoundCue* SkillSound;
	UPROPERTY(EditAnywhere)
	class UParticleSystem* SkillEffect;
	UPROPERTY(EditAnywhere)
	class UAnimMontage* ActivateSkillAnimation;
	UPROPERTY(EditAnywhere)
	class UAnimMontage* DeactivateSkillAnimation;

private:
	
	

	
	

};
