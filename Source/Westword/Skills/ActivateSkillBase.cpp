// Fill out your copyright notice in the Description page of Project Settings.


#include "Skills/ActivateSkillBase.h"



AActivateSkillBase::AActivateSkillBase()
{
 	
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

}


void AActivateSkillBase::BeginPlay()
{
	Super::BeginPlay();
	
}



void AActivateSkillBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AActivateSkillBase::ActivateSkill(APawn* Character)
{
}


void AActivateSkillBase::SkillEffectFunction(APawn* Character)
{
	
}

void AActivateSkillBase::DeactivateSkill(APawn* Character)
{
}

