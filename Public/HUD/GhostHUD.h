// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GhostHUD.generated.h"

/**
 * 
 */
UCLASS()
class UE5_LEARNING_API AGhostHUD : public AHUD
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = Ghost)
	TSubclassOf<class UGhostOverlay> GhostOverlayClass;

	UPROPERTY()
	UGhostOverlay* GhostOverlay;

public:
	FORCEINLINE UGhostOverlay* GetGhostOverlay() const { return GhostOverlay;  }
};
