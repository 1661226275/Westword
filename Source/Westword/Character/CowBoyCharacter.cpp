// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CowBoyCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Runtime/Engine/Classes/Animation/AnimInstance.h"
#include "Engine/Engine.h"


// Sets default values
ACowBoyCharacter::ACowBoyCharacter()
{
 
	PrimaryActorTick.bCanEverTick = true;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	ThirdViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdViewCamera"));
	ThirdViewCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	ThirdViewCamera->bUsePawnControlRotation = false;

	Combat = NewObject<UCombatComponent>(this, TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);
	Combat->RegisterComponent();
	WeaponSolts.Init(nullptr, 2);




}

void ACowBoyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ACowBoyCharacter, OverLapWeapon,COND_OwnerOnly);
}
void ACowBoyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(Combat)
	{
		Combat->Character = this;
	}
}

void ACowBoyCharacter::SetOverLapWeapon(AWeaponBase* Weapon)
{
	if (OverLapWeapon)
	{
		OverLapWeapon->SetPickUpWidgetVisibility(false);
	}
	OverLapWeapon = Weapon;
	//���Ƿ��������ʱ�����Ḵ�ƣ��ֶ�����
	if (IsLocallyControlled())
	{
		if (OverLapWeapon)
		{
			OverLapWeapon->SetPickUpWidgetVisibility(true);
			//��־���
			if(GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("SetPickUpWidgetVisibility"));
			}
		}

	}
}



void ACowBoyCharacter::RepNotify_OverLapWeapon(AWeaponBase* LastWeapon)
{
	if (OverLapWeapon)
	{
		OverLapWeapon->SetPickUpWidgetVisibility(true);
	}
	if (LastWeapon)
	{
		LastWeapon->SetPickUpWidgetVisibility(false);
	}
}


void ACowBoyCharacter::AimOffset(float DeltaTime)
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f)
	{
		//map pitch from 270-360 to -90-0
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

// Called when the game starts or when spawned
void ACowBoyCharacter::BeginPlay()
{
	Super::BeginPlay();

	//��������
	if (HasAuthority() && WeaponClass)
	{
		WeaponSolts[0] = GetWorld()->SpawnActor<ARangeWeapon>(WeaponClass);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("No WeaponClass")));
	}

	if (WeaponSolts[0])
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Spawn Weapon Name is %s"), *WeaponSolts[0]->GetName()));
		}
		const USkeletalMeshSocket* HolsterSocket = GetMesh()->GetSocketByName("HolsterSocket");
		if (HolsterSocket)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Get HolsterSocket")));
			}
			HolsterSocket->AttachActor(WeaponSolts[0], GetMesh());
			WeaponSolts[0]->SetOwner(this);
			WeaponSolts[0]->SetWeaponState(EWeaponState::EWS_PickUp);
		}
	}
	
}


void ACowBoyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AimOffset(DeltaTime);
}

// Called to bind functionality to input
void ACowBoyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed,this, &ACowBoyCharacter::Jump);
	PlayerInputComponent->BindAction("Slide", IE_Pressed, this, &ACowBoyCharacter::Slide);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ACowBoyCharacter::Slide);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ACowBoyCharacter::Slide);
	PlayerInputComponent->BindAction("EquipRange", IE_Pressed, this, &ACowBoyCharacter::EquipRangeWeaponBottonPressed);
	PlayerInputComponent->BindAction("PickUp", IE_Pressed, this, &ACowBoyCharacter::PickUpBottonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ACowBoyCharacter::AimBottonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ACowBoyCharacter::AimBottonReleased);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACowBoyCharacter::FireBottonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ACowBoyCharacter::FireBottonReleased);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACowBoyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACowBoyCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ACowBoyCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ACowBoyCharacter::LookUp);
}



void ACowBoyCharacter::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		ForwardInput = Value;
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}
	
}

void ACowBoyCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		RightInput = Value;
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);
	}

}
void ACowBoyCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ACowBoyCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}
//����Ķ��������漰��״̬����
void ACowBoyCharacter::Jump()
{
	if (Combat->Player_State != ECharacterState::CharacterState_Death)
	{
		Super::Jump();
		//������̫�沥��״̬
		SetPlayingMantogeState(EPlayingMantoge::PlayingMantoge_Blank);
	}
	
}

void ACowBoyCharacter::Slide()
{
	ServerSlide();
}

void ACowBoyCharacter::ServerSlide_Implementation()
{
	MultiCastSlide();
}

void ACowBoyCharacter::MultiCastSlide_Implementation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance != nullptr && PlayingMantogeState == EPlayingMantoge::PlayingMantoge_Blank)
	{
		AnimInstance->Montage_Play(SlideMontage, 1.f);
		SetPlayingMantogeState(EPlayingMantoge::PlayingMantoge_Slide);
	}
}

void ACowBoyCharacter::StartSprint()
{
	
}

void ACowBoyCharacter::EndSprint()
{
}

void ACowBoyCharacter::EquipRangeWeaponBottonPressed()
{
	ServerEquipRangeWeapon();
}

void ACowBoyCharacter::ServerEquipRangeWeapon_Implementation()
{
	MultiCastEquipRangeWeapon();
}

void ACowBoyCharacter::MultiCastEquipRangeWeapon_Implementation()
{
	//װ������,����������ж�
	//1.Զ���������������������ڿ���״̬��װ������
	//2.Զ���������������������ڳ���Զ������������Զ������
	//3.Զ���������������������ڳ��н�ս�����������ս������װ��Զ������
	//4.Զ�����������������޷�װ��
	if (Combat)
	{
		//���1
		if (WeaponSolts[0] && WeaponType == EWeaponType::WeaponType_None)
		{
			Combat->EquipWeapon(WeaponSolts[0]);
			WeaponSolts[0]->PlayEquipMontage();
		}

	}
}

void ACowBoyCharacter::PickUpBottonPressed()
{
}

void ACowBoyCharacter::AimBottonPressed()
{
	if (Combat)
	{
		Combat->SetPlayerState(ECharacterState::CharacterState_Aim);
	}
}

void ACowBoyCharacter::AimBottonReleased()
{
	if (Combat)
	{
		Combat->SetPlayerState(ECharacterState::CharacterState_Norm);
	}
}

void ACowBoyCharacter::FireBottonPressed()
{
	if (Combat)
	{
		Combat->FireBottonPressed(true);
	}
}

void ACowBoyCharacter::FireBottonReleased()
{
	if (Combat)
	{
		Combat->FireBottonPressed(false);
	}
}



