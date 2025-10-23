// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/** Main log category used across the project */
DECLARE_LOG_CATEGORY_EXTERN(LogWestword, Log, All);


#define ECC_SkeletalMesh ECollisionChannel::ECC_GameTraceChannel1
#define ECC_MeleeTraceChannel ECollisionChannel::ECC_GameTraceChannel2
#define ECC_HitBox ECollisionChannel::ECC_GameTraceChannel3