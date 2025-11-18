// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DataType/Team.h"
#include "CowBoyPlayerState.generated.h"

// 委托，当Character设置时通知
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterSet, ACowBoyCharacter*, Character);
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
	float KillHunterNum = 0.f;
	float KillMonsterNum = 0;
	float KillBossNum = 0;
	float Bounty = 0;
	float CrystalShard = 0;
	float Crystal = 0;

	
	
	UPROPERTY(BlueprintAssignable)
	FOnCharacterSet OnCharacterSet;


private:
	class ACowBoyCharacter* Character;
	class ACowBoyPlayerController* Controller;

	UPROPERTY(Replicated)
	ETeam Team = ETeam::ET_NoTeam;

public:
	ETeam GetTeam()const { return Team; }
	void SetTeam(ETeam TEamToSet) { Team = TEamToSet; }

	//设置Character并绑定委托
	void SetCowBoyCharacter(ACowBoyCharacter* NewCharacter);
	void SetCowBoyPlayerController(ACowBoyPlayerController* NewController);

	ACowBoyCharacter* GetCowBoyCharacter() const { return Character; }
	ACowBoyPlayerController* GetCowBoyPlayerController() const { return Controller; }

};
