// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/BuffeComponent.h"
#include "Character/CowBoyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
UBuffeComponent::UBuffeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	
}

void UBuffeComponent::Heal(float HealAmount, float HealingTime)
{
	if (Character == nullptr || Character->IsElimmed()) return;

	// 停止之前的治疗定时器
	GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);

	bHealing = true;
	TotalHealAmount = HealAmount;
	RemainingHealingTime = HealingTime;

	// 计算每秒治疗量
	HealPerSecond = HealAmount / HealingTime;

	// 立即执行一次治疗（第0秒）
	HealTick();

	// 设置每秒治疗一次的定时器
	if (RemainingHealingTime > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(
			HealTimerHandle,
			this,
			&UBuffeComponent::HealTick,
			1.0f, // 每秒一次
			true // 循环
		);
	}
}

void UBuffeComponent::HealTick()
{
	if (!bHealing || Character == nullptr || Character->IsElimmed())
	{
		// 停止定时器
		GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);
		return;
	}

	// 应用治疗
	Character->SetHealth(FMath::Clamp(Character->GetHealth() + HealPerSecond, 0.f, Character->GetMaxHealth()));
	Character->UpdateHUDHealth();

	// 减少剩余治疗时间
	RemainingHealingTime -= 1.0f;

	// 检查是否完成治疗
	if (RemainingHealingTime <= 0.f || Character->GetHealth() >= Character->GetMaxHealth())
	{
		bHealing = false;
		GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);
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

	
}

