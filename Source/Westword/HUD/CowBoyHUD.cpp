// Fill out your copyright notice in the Description page of Project Settings.

#include "HUD/CowBoyHUD.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "CharacterOverlay.h"
#include "CharacterDeBuffWidget.h"
#include "DamageWidget.h"
#include "Announcement.h"
#include"ElimAnnouncement.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/HorizontalBox.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CanvasPanel.h"
#include "PlayerController/CowBoyPlayerController.h"





void ACowBoyHUD::BeginPlay()
{
	Super::BeginPlay();
	
}



void ACowBoyHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
		CharacterOverlay->TeammatePlayerCanvasPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}



void ACowBoyHUD::AddCharacterDeBuffWidget()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && CharacterDeBuffClass && CharacterDeBuffWidget == nullptr)
	{
		CharacterDeBuffWidget = CreateWidget<UCharacterDeBuffWidget>(PlayerController, CharacterDeBuffClass);
		CharacterDeBuffWidget->AddToViewport();

	}
}

void ACowBoyHUD::RemoveCharacterDeBuffWidget()
{
	if (CharacterDeBuffWidget)
	{
		CharacterDeBuffWidget->RemoveFromParent();
		CharacterDeBuffWidget = nullptr;
	}
}

void ACowBoyHUD::AddCharacterHurtHUD()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && CharacterDamageClass)
	{
		CharacterHurtHUD = CreateWidget<UDamageWidget>(PlayerController, CharacterDamageClass);
		CharacterHurtHUD->AddToViewport();

	}
}

void ACowBoyHUD::RemoveCharacterHurtHUD()
{
	if (CharacterHurtHUD)
	{
		CharacterHurtHUD->RemoveFromParent();
		CharacterHurtHUD = nullptr;
	}
}

void ACowBoyHUD::AddDamageEffect()
{
	if (CharacterHurtHUD)
	{
		CharacterHurtHUD->PlayDamageEffect();
	}
}

void ACowBoyHUD::AddElimAnnouncement(FString Attacker, FString Victim)
{
	OwningPlayer = OwningPlayer == nullptr ? GetOwningPlayerController() : OwningPlayer;
	if (OwningPlayer && AnnouncementClass)
	{
		UElimAnnouncement* ElimAnnouncemnetWidget = CreateWidget<UElimAnnouncement>(OwningPlayer, ElimAnnouncementClass);
		
		if (ElimAnnouncemnetWidget)
		{
			ElimAnnouncemnetWidget->SetElimAnnouncementText(Attacker, Victim);
			ElimAnnouncemnetWidget->AddToViewport();
			for (UElimAnnouncement* Msg : ElimMessage)
			{
				if (Msg && Msg->AnnouncementBox)
				{
					UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Msg->AnnouncementBox);
					if (CanvasSlot)
					{
						FVector2D Position = CanvasSlot->GetPosition();
						FVector2D NewPosition(Position.X, Position.Y - CanvasSlot->GetSize().Y);
						CanvasSlot->SetPosition(NewPosition);
					}
					
				}
			}


			ElimMessage.Add(ElimAnnouncemnetWidget);
			FTimerHandle ElimMsgTimer;
			FTimerDelegate ElimMsgDelegate;
			ElimMsgDelegate.BindUFunction(this, FName("ElimAnnouncementTimerFinished"), ElimAnnouncemnetWidget);
			GetWorldTimerManager().SetTimer(
				ElimMsgTimer,
				ElimMsgDelegate,
				ElimAnnouncementTime,
				false
			);
		}
	}
}



void ACowBoyHUD::ElimAnnouncementTimerFinished(UElimAnnouncement* MsgToRemove)
{
	if (MsgToRemove)
	{
		MsgToRemove->RemoveFromParent();
	}
}

void ACowBoyHUD::AddAnnouncement()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && AnnouncementClass && Announcement == nullptr)
	{
		Announcement = CreateWidget<UAnnouncement>(PlayerController, AnnouncementClass);
		Announcement->AddToViewport();
	}
}

void ACowBoyHUD::DrawHUD()
{
	Super::DrawHUD();
	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		float SpreadScaled = HUDPackage.CrosshairSpread * CrosshairSpreadMax;
		if (HUDPackage.CrosshairsCenter)
		{
			FVector2D Spread(0.f, 0.f);
			DrawCrosshairs(HUDPackage.CrosshairsCenter, ViewportCenter, Spread,HUDPackage.CrosshairColor);
		}
		if (HUDPackage.CrosshairsLeft)
		{
			FVector2D Spread(-SpreadScaled, 0.f);
			DrawCrosshairs(HUDPackage.CrosshairsLeft, ViewportCenter, Spread, HUDPackage.CrosshairColor);
		}
		if (HUDPackage.CrosshairsRight)
		{
			FVector2D Spread(SpreadScaled, 0.f);
			DrawCrosshairs(HUDPackage.CrosshairsRight, ViewportCenter, Spread, HUDPackage.CrosshairColor);
		}
		if (HUDPackage.CrosshairsTop)
		{
			FVector2D Spread(0.f, -SpreadScaled);
			DrawCrosshairs(HUDPackage.CrosshairsTop, ViewportCenter, Spread, HUDPackage.CrosshairColor);
		}
		if (HUDPackage.CrosshairsBottom)
		{
			FVector2D Spread(0.f, SpreadScaled);
			DrawCrosshairs(HUDPackage.CrosshairsBottom, ViewportCenter, Spread, HUDPackage.CrosshairColor);
		}
	}
}



void ACowBoyHUD::DrawCrosshairs(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(
		ViewportCenter.X - (TextureWidth / 2.f) + Spread.X,
		ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y
	);
	DrawTexture(
		Texture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.f,
		0.f,
		1.f,
		1.f,
		CrosshairColor
	);
}
void ACowBoyHUD::CreateMiniMapWidget_Implementation()
{

}

void ACowBoyHUD::SetTeammatesUIVisible(bool bIsVisible)
{
	if (bIsVisible)
	{
		CharacterOverlay->TeammatePlayerCanvasPanel->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		CharacterOverlay->TeammatePlayerCanvasPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}


