// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial = 0,
	EWS_PickUp,
	EWS_Equipped,
	EWS_Drop,
	EWS_MAX
};

UENUM(BlueprintType)
enum class EDeathpose : uint8
{
	EDP_DieRight = 0,
	EDP_DieLeft,
	EDP_DieBack1,
	EDP_DieBack2,
	EDP_DieShootHeadBack,
	EDP_DieShootHeadFront,
	EDP_DieShootHeadRight1,
	EDP_DieShootHeadRight2
};
//定义三个枚举变量表示蒙太奇播放状态,当前角色状态,武器装备状态
UENUM(BlueprintType)
enum class EPlayingMantoge : uint8
{
	PlayingMantoge_Blank = 0,
	PlayingMantoge_Slide,
	PlayingMantoge_EquipWeapon,
	PlayingMantoge_UnEquipWeapon,
	PlayingMantoge_Attack,
	PlayingMantoge_DeBuff,
	PlayingMantoge_Death,
	PlayingMantoge_UsingAbility,

};

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	CharacterState_Norm = 0,
	CharacterState_Aim,
	CharacterState_DeBuff,
	CharacterState_Buff,
	CharacterState_Death,

};
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	WeaponType_None = 0,
	WeaponType_Melee,
	WeaponType_Gun,

};

/**
 * 
 */
class WESTWORD_API EnumData
{
public:
	EnumData();
	~EnumData();
};
