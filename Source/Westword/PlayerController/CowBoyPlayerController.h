// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CowBoyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class WESTWORD_API ACowBoyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDSan(float San, float MaxSan);
	void SetHUDScore(float Score);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDMatchCountdown(float CountdownTime);
	void SetHUDAnnouncementCountdown(float CountdownTime);
	void SetRangeWeaponHUDVisible(bool bIsVisible);
	void SetMeleeWeaponHUDVisible(bool bIsVisible);
	void SetCharacterDeBuffHUD();
	void DestoryCharacterDeBuffHUD();
	void SetCharacterHurtHUD();
	void DestoryCharacterHurtHUD();
	void AddDamageEffect();

	void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual float GetServerTime();//Get current server time

	virtual void ReceivedPlayer() override;
	void OnMatchStateSet(FName State, bool bTeamsMatch = false);
	
protected:
	virtual void BeginPlay() override;
	void HandleMatchHasStarted(bool bTeamsMatch = false);
	void HandleWarmup();
	void HandleCooldown();
	void SetHUDTime();
	void PollInit();
	/*
	* Sync time on client and server
	*/
	UFUNCTION(Server,Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);
	//Report current server time to client
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedRequest);

	float ClientServerDelta = 0.f;//Difference between client and server time

	UPROPERTY(EditAnywhere, Category = "Time")
	float TimeSyncFrequency = 5.f;//How often to sync time

	float TimeSyncRunningTime = 0.f;

	void CheckTimeSync(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientJoinMidGame(FName State, float Warmup, float Match, float StartingTime);



private:
	class ACowBoyHUD* CowboyHUD;
	
	float MatchTime = 0.f;
	float WarmupTime = 0.f;
	float LevelStartingTime = 0.f;
	uint32 CountdownInt = 0;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;


	

	float HUDHealth;
	bool bInitializedHealth = false;
	float HUDMaxHealth;
	float HUDSan;
	bool bInitializedSan = false;
	float HUDMaxSan;
	float HUDScore;
	bool bInitializedScore = false;
};
