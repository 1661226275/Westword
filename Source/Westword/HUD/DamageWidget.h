// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageWidget.generated.h"

/**
 * 
 */
UCLASS()
class WESTWORD_API UDamageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void AddDamageEffect();

	UFUNCTION(BlueprintNativeEvent)
	void PlayDamageEffect();

private:
	
	
};
