// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CowBoyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class WESTWORD_API ACowBoyPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	virtual void OnRep_Score() override;
	void AddToScore(float ScoreDelta);

private:
	class ACowBoyCharacter* Character;
	class ACowBoyPlayerController* Controller;
};
