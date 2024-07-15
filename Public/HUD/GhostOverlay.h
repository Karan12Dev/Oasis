// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GhostOverlay.generated.h"


class UProgressBar;
class UTextBlock;

UCLASS()
class UE5_LEARNING_API UGhostOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);
	void SetGold(int32 Gold);
	void SetSouls(int32 Souls);



private:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaProgressBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GoldCount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SoulsCount;
};
