// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/GhostHUD.h"
#include "HUD/GhostOverlay.h"

void AGhostHUD::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller && GhostOverlayClass)
		{
			GhostOverlay = CreateWidget<UGhostOverlay>(Controller, GhostOverlayClass);
			GhostOverlay->AddToViewport();
		}
	}
}
