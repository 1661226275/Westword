// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skills/ActivateSkillBase.h"
#include "BeastInstinct.generated.h"

/**
 * 
 */
UCLASS()
class WESTWORD_API ABeastInstinct : public AActivateSkillBase
{
	GENERATED_BODY()

public:
	ABeastInstinct();
	virtual void ActivateSkill(class APawn* Character) override;
	virtual void SkillEffectFunction(class APawn* Character) override;
	virtual void DeactivateSkill(class APawn* Character) override;
	void FinishCooldown();
	void ServerActivateSkill(class APawn* Character);
	
protected:


private:

	
	UFUNCTION(Server, Reliable)
	void ServerDeactivateSkill(class APawn* Character);
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastActivateSkill(class APawn* Character);
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastDeactivateSkill(class APawn* Character);
	UPROPERTY(EditAnywhere)
	float BaseSpeedBuff =1600.f ;
	UPROPERTY(EditAnywhere)
	float JumpZVelocityBuff = 4000.f;


};
