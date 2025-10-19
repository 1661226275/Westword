// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/BeastInstinct.h"
#include "BeastInstinct.h"
#include "ActivateSkillBase.h"
#include "Character/CowBoyCharacter.h"
#include "Components/BuffeComponent.h"

void ABeastInstinct::ActivateSkill(APawn* Character)
{
}

void ABeastInstinct::ServerActivateSkill_Implementation(APawn* Character)
{
	SkillEffectFunction(Character);
	MultiCastActivateSkill(Character);
}
void ABeastInstinct::MultiCastActivateSkill_Implementation(APawn* Character)
{
	SkillEffectFunction(Character);	 
}

void ABeastInstinct::ServerDeactivateSkill_Implementation(APawn* Character)
{

}
void ABeastInstinct::DeactivateSkill(APawn* Character)
{
}

void ABeastInstinct::MultiCastDeactivateSkill_Implementation(APawn* Character)
{
}

void ABeastInstinct::SkillEffectFunction(APawn* Character)
{
	ACowBoyCharacter* CowBoyCharacter = Cast<ACowBoyCharacter>(Character);
	if (CowBoyCharacter)
	{
		UBuffeComponent* Buff = CowBoyCharacter->GetBuffComponent();
		if (Buff)
		{
			Buff->BuffSpeed(BaseSpeedBuff, SkillDuration);
			Buff->BuffJump(JumpZVelocityBuff, SkillDuration);
		}
	}
}