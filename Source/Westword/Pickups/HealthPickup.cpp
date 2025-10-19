// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/HealthPickup.h"
#include "HealthPickup.h"
#include "Character/CowBoyCharacter.h"

AHealthPickup::AHealthPickup()
{
	bReplicates = true;
}

void AHealthPickup::Interact(APawn* User)
{
	//播放交互动画
	//交互效果，只在服务器上调用
	InteractEffect(User);
	
}

void AHealthPickup::InteractEffect(APawn* User)
{
	//交互效果，只在服务器上调用
	ServerInteractEffect(User);
}

void AHealthPickup::ServerInteractEffect_Implementation(APawn* User)
{
	ACowBoyCharacter* CowBoyCharacter = Cast<ACowBoyCharacter>(User);
	if(CowBoyCharacter)
	{
		UBuffeComponent* Buff = CowBoyCharacter->GetBuffComponent();
		if(Buff)
		{
			Buff->Heal(HealthAmount, HealingTime);
		}
	}
}


