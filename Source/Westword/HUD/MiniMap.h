// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MiniMap.generated.h"

/**
 * 
 */
UCLASS()
class WESTWORD_API UMiniMap : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category = "NativeEvent")
	void CreateFriendIcon(const TArray<class ACowBoyPlayerState*>& Friendlies);
	void CreateFriendIcon_Implementation(const TArray<class ACowBoyPlayerState*>& Friendlies);
	






	
};
