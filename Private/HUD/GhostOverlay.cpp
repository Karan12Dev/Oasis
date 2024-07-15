// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/GhostOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UGhostOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void UGhostOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(Percent);
	}
}

void UGhostOverlay::SetGold(int32 Gold)
{
	if (GoldCount)
	{
		const FString String = FString::Printf(TEXT("%d"), Gold);
		const FText Text = FText::FromString(String);
		GoldCount->SetText(Text);
	}
}

void UGhostOverlay::SetSouls(int32 Souls)
{
	if (SoulsCount)
	{
		const FString String = FString::Printf(TEXT("%d"), Souls);
		const FText Text = FText::FromString(String);
		SoulsCount->SetText(Text);
	}
}
