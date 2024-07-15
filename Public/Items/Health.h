// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Health.generated.h"

/**
 * 
 */
UCLASS()
class UE5_LEARNING_API AHealth : public AItem
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category = "Health")
	int32 Health;


protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherCom, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;


public:
	FORCEINLINE int32 GetHealth() const { return Health; }
	FORCEINLINE void SetHealthPoints(int32 HealthPoints) { Health = HealthPoints; }
};
