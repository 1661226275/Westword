// Fill out your copyright notice in the Description page of Project Settings.



#include "Weapon/MeleeWeaponBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/StaticMeshSocket.h"
#include "Character/CowBoyCharacter.h"
#include "Westword.h"


void AMeleeWeaponBase::EndAttack()
{
    if (Cast<ACowBoyCharacter>(GetOwner()))
    {
        ACowBoyCharacter* Character = Cast<ACowBoyCharacter>(GetOwner());
        Character->GetWorldTimerManager().ClearTimer(AttackTimeHandle);
        Character->SetPlayingMantogeState(EPlayingMantoge::PlayingMantoge_Blank);
    }

}

void AMeleeWeaponBase::OnRep_Owner()
{
	Super::OnRep_Owner();
	if (Owner == nullptr)
	{
		CowBoyOwnerCharacter = nullptr;
		CowBoyOwnerController = nullptr;
	}
}





void AMeleeWeaponBase::MeleeHitChannelTrace()
{
	FHitResult TraceHitResult;
    FVector DamageStart(0, 0, 0);
    FVector DamageEnd(0, 0, 0);
	const UStaticMeshSocket* DamageSocketStart = GetWeaponMesh()->GetSocketByName("DamageSocketStart");
	if (DamageSocketStart)
	{
		FTransform SocketTransform;
		DamageSocketStart->GetSocketTransform(SocketTransform, GetWeaponMesh());
        DamageStart = SocketTransform.GetLocation();
	}
	const UStaticMeshSocket* DamageSocketEnd = GetWeaponMesh()->GetSocketByName("DamageSocketEnd");
	if (DamageSocketEnd)
	{
		FTransform SocketTransform;
		DamageSocketEnd->GetSocketTransform(SocketTransform, GetWeaponMesh());
        DamageEnd = SocketTransform.GetLocation();
	}
    // ���ø��ٲ���
    ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_MeleeTraceChannel); // ʹ�ú��ʵ���ײͨ��
    TArray<AActor*> ActorsToIgnore;

    // ������������ͳ�����
    ActorsToIgnore.Add(this);
    if (GetOwner())
    {
        ActorsToIgnore.Add(GetOwner());
    }

    //Sphere Channel Trace
    bool TraceResult = UKismetSystemLibrary::SphereTraceSingle(
        this,                                    // World Context
        DamageStart,                             // Start Location
        DamageEnd,                               // End Location  
        DamageTraceRange,                        // Sphere Radius
        TraceChannel,                            // Trace Channel
        false,                                   // bTraceComplex
        ActorsToIgnore,                          // Actors to Ignore
        EDrawDebugTrace::ForOneFrame,            // Draw Debug Type 
        TraceHitResult,                          // Out Hit Result
        true,                                    // bIgnoreSelf
        FLinearColor::Red,                       // Trace Color
        FLinearColor::Green,                     // Hit Color
        2.0f                                     // Draw Time
    );

    // �������н��
    if (TraceResult)
    {
        AActor* HitActor = TraceHitResult.GetActor();
        if (HitActor)
        {
            // Ӧ���˺��򴥷�Ч��
            UE_LOG(LogTemp, Log, TEXT("Hit Actor: %s"), *HitActor->GetName());
            if (HitActor != LastHitActor)
            {
                UGameplayStatics::ApplyDamage(
                    HitActor,
                    Damage, // �˺�ֵ
                    GetInstigatorController(),
                    this,
                    UDamageType::StaticClass()
                );
            }
      
            
        }
    }
}

void AMeleeWeaponBase::HandleAttack()
{
    if (Cast<ACowBoyCharacter>(GetOwner()))
    {
        //�ڷ����������ö�ʱ������MeleeHitChannelTrace��ÿ��0.3�����һ�Σ�ͨ������֪ͨ����EndAttackȡ����ʱ��
        Cast<ACowBoyCharacter>(GetOwner())->GetWorldTimerManager().SetTimer(
            AttackTimeHandle,
            this, &AMeleeWeaponBase::MeleeHitChannelTrace,
            0.3f,
            true
        );
    }
    
    
}



void AMeleeWeaponBase::PlayAttackMontage()
{
    ACowBoyCharacter* Character = Cast<ACowBoyCharacter>(GetOwner());
    UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
    if (AnimInstance && AttackMontage)
    {
        AnimInstance->Montage_Play(AttackMontage);
        FName SectionName = FName("MeleeAtt");
        AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
    }
}

void AMeleeWeaponBase::PlayEquipMontage()
{
    Super::PlayEquipMontage();
    ACowBoyCharacter* Character = Cast<ACowBoyCharacter>(GetOwner());
    UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
    if (AnimInstance != nullptr && (Character->PlayingMantogeState == EPlayingMantoge::PlayingMantoge_Blank ||
        Character->PlayingMantogeState == EPlayingMantoge::PlayingMantoge_Slide))
    {
        AnimInstance->Montage_Play(EquipAnimMontage, 1.f);
        Character->SetPlayingMantogeState(EPlayingMantoge::PlayingMantoge_EquipWeapon);
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("RangeWeapon Playing Equip Montage")));
        }
    }
}

void AMeleeWeaponBase::PlayUnEquipMontage()
{
    Super::PlayUnEquipMontage();
    ACowBoyCharacter* Character = Cast<ACowBoyCharacter>(GetOwner());
    UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
    if (AnimInstance != nullptr && (Character->PlayingMantogeState == EPlayingMantoge::PlayingMantoge_Blank ||
        Character->PlayingMantogeState == EPlayingMantoge::PlayingMantoge_Slide))
    {
        AnimInstance->Montage_Play(UnequipAnimMontage, 1.f);
        Character->SetPlayingMantogeState(EPlayingMantoge::PlayingMantoge_UnEquipWeapon);
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("RangeWeapon Playing UnEquip Montage")));
        }
    }
}
