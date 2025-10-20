// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/BeastInstinct.h"
#include "BeastInstinct.h"
#include "ActivateSkillBase.h"
#include "Character/CowBoyCharacter.h"
#include "Components/BuffeComponent.h"

void ABeastInstinct::ActivateSkill(APawn* Character)
{
	ServerActivateSkill(Character);
}

void ABeastInstinct::ServerActivateSkill_Implementation(APawn* Character)
{
	//�����ͷ������ж�
	if (bIsCooldown)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString("Skill is CoolDown"));
		return;
	}
	bIsCooldown = true;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("CoolDownTime: %.2f"), SkillCooldown));
	Cast<ACowBoyCharacter>(Character)->GetWorldTimerManager().SetTimer(TimerHandle_Cooldown, this, &ABeastInstinct::FinishCooldown, SkillCooldown, false);
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
		if (HasAuthority())
		{
			UBuffeComponent* Buff = CowBoyCharacter->GetBuffComponent();
			if (Buff)
			{
				Buff->BuffSpeed(BaseSpeedBuff, SkillDuration);
				Buff->BuffJump(JumpZVelocityBuff, SkillDuration);
			}
			CowBoyCharacter->HandleSanChange(SanCost);
		}
		UAnimInstance* AnimInstance = CowBoyCharacter->GetMesh()->GetAnimInstance();
		if (AnimInstance && ActivateSkillAnimation)
		{
			AnimInstance->Montage_Play(ActivateSkillAnimation);
			FName SectionName = FName("BeastInstinct");
			AnimInstance->Montage_JumpToSection(SectionName, ActivateSkillAnimation);
			CowBoyCharacter->SetPlayingMantogeState(EPlayingMantoge::PlayingMantoge_UsingAbility);
		}
		
	}
}

void ABeastInstinct::FinishCooldown()
{
	bIsCooldown = false;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString("Finish cooldown"));
}