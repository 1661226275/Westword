// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DataType/Team.h"
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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	uint8 KillHunterNum = 0;
	uint8 KillMonsterNum = 0;
	uint8 KillBossNum = 0;
	uint8 Bounty = 0;
	uint8 CrystalShard = 0;
	uint8 Crystal = 0;
private:
	class ACowBoyCharacter* Character;
	class ACowBoyPlayerController* Controller;

	UPROPERTY(Replicated)
	ETeam Team = ETeam::ET_NoTeam;

public:
	ETeam GetTeam()const { return Team; }
	void SetTeam(ETeam TEamToSet) { Team = TEamToSet; }
};
