// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/BuffeComponent.h"
#include "Character/CowBoyCharacter.h"
UBuffeComponent::UBuffeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	
}


void UBuffeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


void UBuffeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

