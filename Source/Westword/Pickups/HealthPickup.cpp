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
	
}

void AHealthPickup::InteractEffect(APawn* User)
{
	//用于动画通知
}


