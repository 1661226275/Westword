// Fill out your copyright notice in the Description page of Project Settings.


#include "CowBoyNameWidget.h"
#include "Components/TextBlock.h"

void UCowBoyNameWidget::SetNameText(FString SelfName)
{
	FString SelfNameText = FString::Printf(TEXT("%s"), *SelfName);
	if (NameText)
	{
		NameText->SetText(FText::FromString(SelfNameText));
	}
}
