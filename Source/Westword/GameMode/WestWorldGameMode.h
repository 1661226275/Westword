// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "WestWorldGameMode.generated.h"

/**
 * 
 */
UCLASS()
class WESTWORD_API AWestWorldGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void PlayerEliminated(class ACowBoyCharacter* ElimmedCharacter, class ACowBoyPlayerController* VictimController,AController* AttackController);
	virtual void RequestRespawn(class ACharacter* ElimmedCharacter, class AController* ElimmedController);
	
};
