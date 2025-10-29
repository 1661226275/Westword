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
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerState/CowBoyPlayerState.h"
#include "Components/LagCompenstionComponent.h"

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

	LagCompensation = CreateDefaultSubobject<ULagCompenstionComponent>(TEXT("LagCompensation"));
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	WeaponSolts.Init(nullptr, 2);
	EquipWeaponSocket.Add(0, FName("HolsterSocket"));
	EquipWeaponSocket.Add(1, FName("HolsterMeleeSocket"));
	SkillsArray.Init(nullptr, 2);

	/*
	*  ��ײ��
	*/
	head = CreateDefaultSubobject<UBoxComponent>(TEXT("head"));
	head->SetupAttachment(GetMesh(), FName("head"));
	head->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("head"), head);

	pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("pelvis"));
	pelvis->SetupAttachment(GetMesh(), FName("pelvis"));
	pelvis->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("pelvis"), pelvis);

	spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_02"));
	spine_02->SetupAttachment(GetMesh(), FName("spine_02"));
	spine_02->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("spine_02"), spine_02);

	spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_03"));
	spine_03->SetupAttachment(GetMesh(), FName("spine_03"));
	spine_03->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("spine_03"), spine_03);

	upperarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_l"));
	upperarm_l->SetupAttachment(GetMesh(), FName("upperarm_l"));
	upperarm_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("upperarm_l"), upperarm_l);

	upperarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_r"));
	upperarm_r->SetupAttachment(GetMesh(), FName("upperarm_r"));
	upperarm_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("upperarm_r"), upperarm_r);

	lowerarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_l"));
	lowerarm_l->SetupAttachment(GetMesh(), FName("lowerarm_l"));
	lowerarm_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("lowerarm_l"), lowerarm_l);

	lowerarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_r"));
	lowerarm_r->SetupAttachment(GetMesh(), FName("lowerarm_r"));
	lowerarm_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("lowerarm_r"), lowerarm_r);

	hand_l = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_l"));
	hand_l->SetupAttachment(GetMesh(), FName("hand_l"));
	hand_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("hand_l"), hand_l);

	hand_r = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_r"));
	hand_r->SetupAttachment(GetMesh(), FName("hand_r"));
	hand_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("hand_r"), hand_r);

	thigh_l = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_l"));
	thigh_l->SetupAttachment(GetMesh(), FName("thigh_l"));
	thigh_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("thigh_l"), thigh_l);

	thigh_r = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_r"));
	thigh_r->SetupAttachment(GetMesh(), FName("thigh_r"));
	thigh_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("thigh_r"), thigh_r);

	calf_l = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_l"));
	calf_l->SetupAttachment(GetMesh(), FName("calf_l"));
	calf_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("calf_l"), calf_l);

	calf_r = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_r"));
	calf_r->SetupAttachment(GetMesh(), FName("calf_r"));
	calf_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("calf_r"), calf_r);

	foot_l = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_l"));
	foot_l->SetupAttachment(GetMesh(), FName("foot_l"));
	foot_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("foot_l"), foot_l);

	foot_r = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_r"));
	foot_r->SetupAttachment(GetMesh(), FName("foot_r"));
	foot_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("foot_r"), foot_r);

	for (auto Box : HitCollisionBoxes)
	{
		if (Box.Value)
		{
			Box.Value->SetCollisionObjectType(ECC_HitBox);
			Box.Value->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			Box.Value->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);

		}
	}
}

void ACowBoyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ACowBoyCharacter, OverLapWeapon, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ACowBoyCharacter, OverLapInteractActor, COND_OwnerOnly);
	DOREPLIFETIME(ACowBoyCharacter, WeaponSolts);
	DOREPLIFETIME(ACowBoyCharacter, Health);
	DOREPLIFETIME(ACowBoyCharacter, San);
	DOREPLIFETIME(ACowBoyCharacter, BeastInstinct);
	DOREPLIFETIME(ACowBoyCharacter, bIsSprinting);
	
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
	if (LagCompensation)
	{
		LagCompensation->Character = this;
		if (Controller)
		{
			LagCompensation->Controller = Cast<ACowBoyPlayerController>(Controller);
		}
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
			AWeaponBase* Weapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass[i]);
			//if (HasAuthority() && IsLocallyControlled())//server����ҳ��е�����
			//{
			//	//���÷���������
			//	Weapon->bUseServerSidleRewind = false;
			//}
			WeaponSolts[i] = Weapon;
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
		CowBoyController = CowBoyController == nullptr ? Cast<ACowBoyPlayerController>(GetController()) : CowBoyController;
		if (CowBoyController->IsLocalController())
		{
			CowBoyController->AddDamageEffect();
		}
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
		if (BeastInstinct)
		{
			BeastInstinct->ActivateSkill(this);
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString("NOSkill2"));
		}
		
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
	if (GetWorldTimerManager().IsTimerActive(EquipDelayTimerHandle)) return;
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
		//���2
		else if (WeaponSolts[0] && WeaponType == EWeaponType::WeaponType_Gun)
		{
			WeaponSolts[0]->PlayUnEquipMontage();
			Combat->UnEquipWeapon();
		}
		//���3
		else if (WeaponSolts[0] && WeaponType == EWeaponType::WeaponType_Melee)
		{
			WeaponSolts[1]->PlayUnEquipMontage();
			Combat->UnEquipWeapon();
			//�ӳ٣��ȴ���̫�沥�����
			float DelayTime = 1.5f; 
			GetWorld()->GetTimerManager().SetTimer(
				EquipDelayTimerHandle,
				this,
				&ACowBoyCharacter::OnMeleeWeaponEquipDelayCompleted,
				DelayTime,
				false 
			);
		}

	}
}

void ACowBoyCharacter::EquipMeleeWeaponBottonPressed()
{
	ServerEquipMeleeWeapon();
}

void ACowBoyCharacter::ServerEquipMeleeWeapon_Implementation()
{
	if (GetWorldTimerManager().IsTimerActive(EquipDelayTimerHandle)) return;
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
		//���2
		else if (WeaponSolts[1] && WeaponType == EWeaponType::WeaponType_Melee)
		{
			WeaponSolts[1]->PlayUnEquipMontage();
			Combat->UnEquipWeapon();

		}
		else if (WeaponSolts[1] && WeaponType == EWeaponType::WeaponType_Gun)
		{
			WeaponSolts[0]->PlayUnEquipMontage();
			Combat->UnEquipWeapon();
			float DelayTime = 1.f; // ������̫�泤�ȵ���
			GetWorld()->GetTimerManager().SetTimer(
				EquipDelayTimerHandle,
				this,
				&ACowBoyCharacter::OnRangeWeaponEquipDelayCompleted,
				DelayTime,
				false 
			);

		}
	}
}

void ACowBoyCharacter::OnRangeWeaponEquipDelayCompleted()
{
	if (WeaponSolts[1] && Combat)
	{
		WeaponSolts[1]->PlayEquipMontage();
		Combat->EquipWeapon(WeaponSolts[1]);
	}

	GetWorld()->GetTimerManager().ClearTimer(EquipDelayTimerHandle);
}

void ACowBoyCharacter::OnMeleeWeaponEquipDelayCompleted()
{
	if (WeaponSolts[0] && Combat)
	{
		WeaponSolts[0]->PlayEquipMontage();
		Combat->EquipWeapon(WeaponSolts[0]);
	}

	GetWorld()->GetTimerManager().ClearTimer(EquipDelayTimerHandle);
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
		if (IsLocallyControlled())
		{
			Combat->bAimButtonPressed = true;
		}
	}
}

void ACowBoyCharacter::AimBottonReleased()
{
	if (Combat)
	{
		Combat->SetPlayerState(ECharacterState::CharacterState_Norm);
		if (IsLocallyControlled())
		{
			Combat->bAimButtonPressed = false;
		}
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

