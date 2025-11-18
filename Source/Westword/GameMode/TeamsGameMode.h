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

	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)override;
	virtual void PlayerEliminated(class ACowBoyCharacter* ElimmedCharacter, class ACowBoyPlayerController* VictimController, AController* AttackController) override;

	// 处理队伍成员生命值变更
	UFUNCTION()
	void HandleTeammateHealthChanged(ACowBoyCharacter* ChangedCharacter, float NewHealth);

	UFUNCTION()
	void HandleTeammateSanChanged(ACowBoyCharacter* ChangedCharacter, float NewSan);

	
protected:
	virtual void HandleMatchHasStarted() override;
	
};
