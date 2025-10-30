// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CowBoyNameWidget.generated.h"

/**
 * 
 */
UCLASS()
class WESTWORD_API UCowBoyNameWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetNameText(FString SelfName);

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NameText;
	
};
