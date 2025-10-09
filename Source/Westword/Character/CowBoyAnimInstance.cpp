// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CowBoyAnimInstance.h"
#include "Character/CowBoyCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UCowBoyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	CowBoyCharacter = Cast<ACowBoyCharacter>(TryGetPawnOwner());
}

void UCowBoyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (CowBoyCharacter == nullptr)
	{
		CowBoyCharacter = Cast<ACowBoyCharacter>(TryGetPawnOwner());
	}
	if (CowBoyCharacter == nullptr)
	{
		return;
	}
	FVector Velocity = CowBoyCharacter->GetVelocity();
	Velocity.Z = 0;
	Speed = Velocity.Size();
	bIsInAir = CowBoyCharacter->GetMovementComponent()->IsFalling();
	bIsAccelerating = CowBoyCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	ForwardInput = CowBoyCharacter->GetForwardInput();
	RightInput = CowBoyCharacter->GetRightInput();
	CowBoyCharacter->SetForWardInput(0);
	CowBoyCharacter->SetRightInput(0);
	bIsAming = CowBoyCharacter->IsAiming();
	Direction = CalculateDirection(Velocity, CowBoyCharacter->GetActorRotation());
	AO_Pitch = CowBoyCharacter->GetAOPitch();

	if (CowBoyCharacter->GetWeapon(0)!=nullptr)
	{
		RightHandIndexSocketTransform = CowBoyCharacter->GetWeapon(0)->GetWeaponMesh()->GetSocketTransform(FName("IndexFingerSocket"), ERelativeTransformSpace::RTS_World);
		FVector RightHandIndexLocation;
		FRotator RightHandIndexRotation;
		CowBoyCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), RightHandIndexSocketTransform.GetLocation(), RightHandIndexSocketTransform.GetRotation().Rotator(), RightHandIndexLocation, RightHandIndexRotation);
		RightHandIndexSocketTransform.SetLocation(RightHandIndexLocation);
		RightHandIndexSocketTransform.SetRotation(RightHandIndexRotation.Quaternion());
	}
}