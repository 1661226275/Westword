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
	//���Ž�������
	
}

void AHealthPickup::InteractEffect(APawn* User)
{
	//���ڶ���֪ͨ
}


