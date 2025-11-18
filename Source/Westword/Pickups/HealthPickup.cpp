// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/HealthPickup.h"
#include "HealthPickup.h"
#include "Character/CowBoyCharacter.h"



void AHealthPickup::Interact(APawn* User)
{
	//播放交互动画
	ACowBoyCharacter* Character = Cast<ACowBoyCharacter>(User);
	
	if (Character)
	{
		UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
		if (AnimInstance&&InteractorAnimation&& Character->PlayingMantogeState==EPlayingMantoge::PlayingMantoge_Blank)
		{
			AnimInstance->Montage_Play(InteractorAnimation);
			FName SectionName = FName("Healing");
			AnimInstance->Montage_JumpToSection(SectionName, InteractorAnimation);
			Character->SetPlayingMantogeState(EPlayingMantoge::PlayingMantoge_UsingAbility);
		}
	}
	
	
}

void AHealthPickup::InteractEffect(APawn* User)
{
	

	
	//交互效果，只在服务器上调用
	if (HasAuthority())
	{
		
		ACowBoyCharacter* CowBoyCharacter = Cast<ACowBoyCharacter>(User);
		
		if (CowBoyCharacter)
		{
			UBuffeComponent* Buff = CowBoyCharacter->GetBuffComponent();
			if (Buff)
			{
				
				Buff->Heal(HealthAmount, HealingTime);
			}
			else
			{
				
			}
		}
		else
		{
			
		}
	}
}





