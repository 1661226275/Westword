// Fill out your copyright notice in the Description page of Project Settings.


#include "CowBoyCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/Engine.h"
#include "Westword/Westword.h"
#include "Runtime/Engine/Classes/Animation/AnimInstance.h"
#include "PlayerController/CowBoyPlayerController.h"
#include "GameMode/WestWorldGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Pickups/Pickup.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerState/CowBoyPlayerState.h"


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

	Combat = CreateDefaultSubobject<UCombatComponent>( TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	Buff = CreateDefaultSubobject<UBuffeComponent>(TEXT("BuffComponent"));
	Buff->SetIsReplicated(true);
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	WeaponSolts.Init(nullptr, 2);
	EquipWeaponSocket.Add(0, FName("HolsterSocket"));
	EquipWeaponSocket.Add(1, FName("HolsterMeleeSocket"));
	SkillsArray.Init(nullptr, 2);

}

void ACowBoyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ACowBoyCharacter, OverLapWeapon, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ACowBoyCharacter, OverLapInteractActor, COND_OwnerOnly);
	DOREPLIFETIME(ACowBoyCharacter, WeaponSolts);
	DOREPLIFETIME(ACowBoyCharacter, Health);
	DOREPLIFETIME(ACowBoyCharacter, San);
}

void ACowBoyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->PrimaryComponentTick.bCanEverTick = true;
		Combat->Character = this;
	}
	if (Buff)
	{
		Buff->Character = this;
		Buff->SetInitialBaseSpeed(GetCharacterMovement()->MaxWalkSpeed);
		Buff->SetInitialJumpZVelocity(GetCharacterMovement()->JumpZVelocity);
	}
}

// Called when the game starts or when spawned
void ACowBoyCharacter::BeginPlay()
{
	Super::BeginPlay();
	//��������
	if (HasAuthority())
	{
		for (int i = 0; i<WeaponClass.Num();i++)
		{
			if (WeaponClass[i] == nullptr) continue;
			WeaponSolts[i] = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass[i]);
			if (WeaponSolts[i])
			{
				if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Spawn Weapon Name is %s"), *WeaponSolts[i]->GetName()));
				FName SocketName = EquipWeaponSocket[i];
				const USkeletalMeshSocket* HolsterSocket = GetMesh()->GetSocketByName(SocketName);
				if (HolsterSocket)
				{
					HolsterSocket->AttachActor(WeaponSolts[i], GetMesh());
					WeaponSolts[i]->SetWeaponState(EWeaponState::EWS_PickUp);
					WeaponSolts[i]->SetOwner(this);
					
				}
			}
		}
		BeastInstinct = GetWorld()->SpawnActor<ABeastInstinct>(SkillsArray[0]);
	}

	UpdateHUDHealth();
	UpdateHUDSan();
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ACowBoyCharacter::ReceiveDamage);
	}

	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}


void ACowBoyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AimOffset(DeltaTime);
	HideCharacterIfCharacterClose();
	PollInit();
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
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("SetPickUpWidgetVisibility"));
			}
		}

	}
}

void ACowBoyCharacter::SetOverLapInteractActor(APickup* Actor)
{
	if (OverLapInteractActor)
	{
		Actor->SetInteractWidgetVisibility(false);
	}
	OverLapInteractActor = Actor;
	if (IsLocallyControlled())
	{
		if (OverLapInteractActor)
		{
			OverLapInteractActor->SetInteractWidgetVisibility(true);
			//��־���
			if (GEngine)
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

void ACowBoyCharacter::RepNotify_OverLapInteractActor(APickup* LastActor)
{
	if (OverLapInteractActor)
	{
		OverLapInteractActor->SetInteractWidgetVisibility(true);
	}
	if(LastActor)
	{
		LastActor->SetInteractWidgetVisibility(false);
	}
}

void ACowBoyCharacter::HideCharacterIfCharacterClose()
{
	if (!IsLocallyControlled()) return;
	if ((ThirdViewCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee= false;

		}
	}
}


void ACowBoyCharacter::PlayDieMontage()
{
	if (DieMontage == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(DieMontage);
		FName SectionName = FName("DieRight");
		AnimInstance->Montage_JumpToSection(SectionName);
		SetPlayingMantogeState(EPlayingMantoge::PlayingMantoge_Death);
	}
}

void ACowBoyCharacter::PlayDieShootHeadMontage()
{
	if (DieMontage == nullptr)return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(DieMontage);
		FName SectionName = FName("DieShootHeadBack");
		AnimInstance->Montage_JumpToSection(SectionName);
		SetPlayingMantogeState(EPlayingMantoge::PlayingMantoge_Death);
	}
}

void ACowBoyCharacter::UpdateHUDHealth()
{
	CowBoyController = CowBoyController == nullptr ? Cast<ACowBoyPlayerController>(GetController()) : CowBoyController;
	if (CowBoyController)
	{
		CowBoyController->SetHUDHealth(Health, MaxHealth);
	}
}

void ACowBoyCharacter::UpdateHUDSan()
{
	CowBoyController = CowBoyController == nullptr ? Cast<ACowBoyPlayerController>(GetController()) : CowBoyController;
	if (CowBoyController)
	{
		CowBoyController->SetHUDSan(San, MaxSan);
	}
}



void ACowBoyCharacter::PollInit()
{
	if(CowBoyPlayerState==nullptr)
	{
		CowBoyPlayerState = GetPlayerState<ACowBoyPlayerState>();
		if(CowBoyPlayerState)
		{
			CowBoyPlayerState->AddToScore(0.f);
		}
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



void ACowBoyCharacter::RequestRespawn()
{
	AWestWorldGameMode* WestWorldGameMode = GetWorld()->GetAuthGameMode<AWestWorldGameMode>();
	if (WestWorldGameMode)
	{
		WestWorldGameMode->RequestRespawn(this,CowBoyController);
	}
}

void ACowBoyCharacter::MulticastRespawn_Implementation()
{
}


void ACowBoyCharacter::Elim()
{
	if (Combat && Combat->EquippedWeapon)
	{
		Combat->EquippedWeapon->Dropped();
	}
	MultCastElim();
}
void ACowBoyCharacter::MultCastElim_Implementation()
{
	bElimmed = true;
	if (!bShootHead)
	{
		PlayDieMontage();
	}
	else 
	{
		PlayDieShootHeadMontage();
	}
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();

	if (CowBoyController)
	{
		DisableInput(CowBoyController);
	}
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_MeleeTraceChannel,ECollisionResponse::ECR_Ignore);
	
}

void ACowBoyCharacter::PlayHitReactMontage()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, FString("PlayHitReactMontage"));
	if (Combat == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName;
		if (Combat->EquippedWeapon)
		{
			switch (Combat->EquippedWeapon->GetWeaponType())
			{
			case EWeaponType::WeaponType_Gun:
			{
				SectionName = FName("HitFront");
				break;
			}
			case EWeaponType::WeaponType_Melee:
			{
				SectionName = FName("EquipMeleeHit");
				break;
			}
			
			}
			
		}
		else
		{
			SectionName = FName("DefaultHit");
		}
		AnimInstance->Montage_JumpToSection(SectionName);
		SetPlayingMantogeState(EPlayingMantoge::PlayingMantoge_Hit);
		
	}
}

void ACowBoyCharacter::PlayDeBuffReactMontage()
{
	if (Combat == nullptr || Combat->Player_State == ECharacterState::CharacterState_Death) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeBuffReactMontage)
	{
		AnimInstance->Montage_Play(DeBuffReactMontage);
		FName SectionName = FName("DeBuffReact");
		AnimInstance->Montage_JumpToSection(SectionName);
		SetPlayingMantogeState(EPlayingMantoge::PlayingMantoge_DeBuff);
	}
}

void ACowBoyCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	UpdateHUDHealth();
	//to do 
	//�����˺�������ȥ���¸���ֵ

	PlayHitReactMontage();
	CowBoyController = CowBoyController == nullptr ? Cast<ACowBoyPlayerController>(GetController()) : CowBoyController;
	if (CowBoyController)
	{
		CowBoyController->AddDamageEffect();
	}

	if (Health <= 0.f)
	{
		AWestWorldGameMode* WestWorldGameMode = GetWorld()->GetAuthGameMode<AWestWorldGameMode>();
		if (WestWorldGameMode)
		{
			CowBoyController = CowBoyController == nullptr ? Cast<ACowBoyPlayerController>(GetController()) : CowBoyController;
			WestWorldGameMode->PlayerEliminated(this, CowBoyController, InstigatedBy);
		}
	}
}

void ACowBoyCharacter::OnRep_Health(float LastHealth)
{

	UpdateHUDHealth();
	if (Health < LastHealth)
	{
		PlayHitReactMontage();
	}

}



void ACowBoyCharacter::HandleSanChange(float DeltaSan)
{
	ServerHandleSanChange(DeltaSan);
}

void ACowBoyCharacter::ServerHandleSanChange_Implementation(float DeltaSan)
{
	if (DeltaSan>0)
	{
		// ����ֵ����
		// ��ʾHUD��Ч��������������ң�
		if (IsLocallyControlled())
		{
			San = FMath::Clamp(San + DeltaSan, 0, MaxSan);
			UpdateHUDSan();
			CowBoyController = CowBoyController == nullptr ? Cast<ACowBoyPlayerController>(GetController()) : CowBoyController;
			if (CowBoyController)
			{
				CowBoyController->SetCharacterDeBuffHUD();
			}
		}
		// ����30��󸯻�ֵ��ʼ���͵ļ�ʱ��
		GetWorldTimerManager().ClearTimer(TimerHandle_SanDecreaseDelay);
		GetWorldTimerManager().SetTimer(TimerHandle_SanDecreaseDelay, this, &ACowBoyCharacter::StartDecreaseSan, 30.0f, false);

		// ���������ֵ�����������Ե�Ѫ����������
		if (San >= 70 && !GetWorldTimerManager().IsTimerActive(TimerHandle_SanDamage))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString("TimerHandle_SanDamage"));
			GetWorldTimerManager().SetTimer(TimerHandle_SanDamage, this, &ACowBoyCharacter::ReceiveSanDamage, 5.0f, true);
		}

		// ��������Զ����ͣ���ֹͣ�Զ�����
		if (GetWorldTimerManager().IsTimerActive(TimerHandle_SanDecrease))
		{
			GetWorldTimerManager().ClearTimer(TimerHandle_SanDecrease);
		}
	}
	else if (DeltaSan<0)
	{
		// ����ֵ����
		San = FMath::Clamp(San + DeltaSan, 0, MaxSan);
		// ������͵���ֵ���£�ֹͣ�����Ե�Ѫ
		if (San < 70)
		{
			if (HasAuthority())
			{
				GetWorldTimerManager().ClearTimer(TimerHandle_SanDamage);
			}
		}
	}
}

void ACowBoyCharacter::ReceiveSanDamage()
{
	// ����˺����������쳣
	TakeDamage(SanDamage, FDamageEvent(), GetController(), this);
}

void ACowBoyCharacter::StartDecreaseSan()
{
	// �������Ͽ�ʼÿ���ӽ��͸���ֵ
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(TimerHandle_SanDecrease, this, &ACowBoyCharacter::DecreaseSan, 5.0f, true);
	}
}

void ACowBoyCharacter::DecreaseSan()
{
	if (San > 0)
	{
		HandleSanChange(-5.f);
		// �ڷ��������޸ĸ��Ʊ��������Զ����Ƶ��ͻ��ˣ����������������OnRep�����������������Ҫ�ֶ�����HUD
		UpdateHUDSan();

		// �������ֵ��Ϊ0��ֹͣ��ʱ��
		if (San <= 0)
		{
			GetWorldTimerManager().ClearTimer(TimerHandle_SanDecrease);
		}
	}
}

void ACowBoyCharacter::OnRep_San(float LastSan)
{
	UpdateHUDSan();
	if (San > 0)
	{
		CowBoyController = CowBoyController == nullptr ? Cast<ACowBoyPlayerController>(GetController()) : CowBoyController;
		if (CowBoyController)
		{
			CowBoyController->SetCharacterDeBuffHUD();
		}
	}
	else
	{
		CowBoyController = CowBoyController == nullptr ? Cast<ACowBoyPlayerController>(GetController()) : CowBoyController;
		if (CowBoyController)
		{
			CowBoyController->DestoryCharacterDeBuffHUD();
		}
	}
	

}


// Called to bind functionality to input
void ACowBoyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACowBoyCharacter::Jump);
	PlayerInputComponent->BindAction("Slide", IE_Pressed, this, &ACowBoyCharacter::Slide);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ACowBoyCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ACowBoyCharacter::EndSprint);
	PlayerInputComponent->BindAction("EquipRange", IE_Pressed, this, &ACowBoyCharacter::EquipRangeWeaponBottonPressed);
	PlayerInputComponent->BindAction("EquipMelee", IE_Pressed, this, &ACowBoyCharacter::EquipMeleeWeaponBottonPressed);
	PlayerInputComponent->BindAction("PickUp", IE_Pressed, this, &ACowBoyCharacter::PickUpBottonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ACowBoyCharacter::AimBottonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ACowBoyCharacter::AimBottonReleased);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ACowBoyCharacter::AttackBottonPressed);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &ACowBoyCharacter::AttackBottonReleased);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ACowBoyCharacter::ReloadBottonPressed);
	PlayerInputComponent->BindAction("Skill2", IE_Pressed, this, &ACowBoyCharacter::ActivateSkill2);

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
void ACowBoyCharacter::ActivateSkill1()
{
}
void ACowBoyCharacter::ActivateSkill2()
{
	if (PlayingMantogeState == EPlayingMantoge::PlayingMantoge_Blank)
	{
		BeastInstinct->ActivateSkill(this);
	}
	
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
	bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = MaxSprintSpeed;
	
}

void ACowBoyCharacter::EndSprint()
{
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void ACowBoyCharacter::EquipRangeWeaponBottonPressed()
{
	ServerEquipRangeWeapon();
}

void ACowBoyCharacter::ServerEquipRangeWeapon_Implementation()
{
	if (Combat && WeaponSolts[0])
	{
		MultiCastEquipRangeWeapon();
	}
	
}

void ACowBoyCharacter::MultiCastEquipRangeWeapon_Implementation()
{
	//װ��Զ������,����������ж�
	//1.Զ���������������������ڿ���״̬��װ������
	//2.Զ���������������������ڳ���Զ������������Զ������
	//3.Զ���������������������ڳ��н�ս�����������ս������װ��Զ������
	//4.Զ�����������������޷�װ��
	if (Combat)
	{
		//���1
		if (WeaponSolts[0] && WeaponType == EWeaponType::WeaponType_None)
		{
			WeaponSolts[0]->PlayEquipMontage();
			Combat->EquipWeapon(WeaponSolts[0]);
		}

	}
}

void ACowBoyCharacter::EquipMeleeWeaponBottonPressed()
{
	ServerEquipMeleeWeapon();
}

void ACowBoyCharacter::ServerEquipMeleeWeapon_Implementation()
{
	if (Combat && WeaponSolts[1])
	{
		MultiCastEquipMeleeWeapon();
	}
}

void ACowBoyCharacter::MultiCastEquipMeleeWeapon_Implementation()
{
	//װ����ս����,����������ж�
	//1.��ս�������������������ڿ���״̬��װ������
	//2.��ս�������������������ڳ��н�ս�����������ս����
	//3.��ս�������������������ڳ���Զ������������Զ��������װ����ս����
	//4.��ս���������������޷�װ��
	if (Combat)
	{
		//���1
		if (WeaponSolts[1] && WeaponType == EWeaponType::WeaponType_None)
		{
			WeaponSolts[1]->PlayEquipMontage();
			Combat->EquipWeapon(WeaponSolts[1]);
			
		}

	}
}

void ACowBoyCharacter::PickUpBottonPressed()
{
	if (OverLapInteractActor)
	{
		OverLapInteractActor->Interact(this);
	}
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

void ACowBoyCharacter::AttackBottonPressed()
{
	if (Combat)
	{
		Combat->AttackBottonPressed(true);
	}
}

void ACowBoyCharacter::AttackBottonReleased()
{
	if (Combat)
	{
		Combat->AttackBottonPressed(false);
	}
}

void ACowBoyCharacter::ReloadBottonPressed()
{
	if (Combat)
	{
		Combat->Reload();
	}
}

