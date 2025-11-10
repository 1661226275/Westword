// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/BuffeComponent.h"
#include "Character/CowBoyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
UBuffeComponent::UBuffeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	
}

void UBuffeComponent::Heal(float HealAmount, float HealingTime)
{
	bHealing = true;
	HealRate = HealAmount / HealingTime;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString("Heal"));
	AmountToHeal += HealAmount;
}

void UBuffeComponent::HealRampUp(float DeltaTime)
{
	if (!bHealing || Character==nullptr||Character->IsElimmed()) return;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString("HealRampUp"));
	const float HealThisFrame = HealRate * DeltaTime;
	Character->SetHealth(FMath::Clamp(Character->GetHealth() + HealThisFrame, 0.f, Character->GetMaxHealth()));
	Character->UpdateHUDHealth();
	AmountToHeal -= HealThisFrame;
	if(AmountToHeal <= 0.f || Character->GetHealth() >= Character->GetMaxHealth())
	{
		bHealing = false;
		AmountToHeal = 0.f;
	}
}

void UBuffeComponent::BuffSpeed(float BuffBaseSpeed, float BuffTime)
{
	if (Character == nullptr) return;
	Character->GetWorldTimerManager().SetTimer(
		SpeedBuffTimer, 
		this, &UBuffeComponent::ResetSpeed, 
		BuffTime
	);
	if(Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BuffBaseSpeed;
	}
}
void UBuffeComponent::ResetSpeed()
{
	if (Character == nullptr) return;
	if(Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = IntialBaseSpeed;
	}
}

void UBuffeComponent::BuffJump(float BuffJumpZVelocity, float BuffTime)
{
	if (Character == nullptr) return;
	Character->GetWorldTimerManager().SetTimer(
		JumpBuffTimer, 
		this, &UBuffeComponent::ResetJump, 
		BuffTime
	);
	if(Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->JumpZVelocity = BuffJumpZVelocity;
	}
}

void UBuffeComponent::ResetJump()
{
	if (Character == nullptr) return;
	if(Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->JumpZVelocity = IntialJumpZVelocity;
	}
}

void UBuffeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}




void UBuffeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HealRampUp(DeltaTime);
}

