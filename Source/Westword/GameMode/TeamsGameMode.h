// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/WestWorldGameMode.h"
#include "TeamsGameMode.generated.h"

/**
 * 
 */
UCLASS()
class WESTWORD_API ATeamsGameMode : public AWestWorldGameMode
{
	GENERATED_BODY()

public:
	virtual void Logout(AController* Exiting) override;

protected:
	virtual void HandleMatchHasStarted() override;
	
};
