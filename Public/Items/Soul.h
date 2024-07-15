// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Soul.generated.h"

/**
 * 
 */
UCLASS()
class UE5_LEARNING_API ASoul : public AItem
{
	GENERATED_BODY()
	
public:
	virtual void Tick(float DeltaTime) override;


protected:
	virtual void BeginPlay() override;
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherCom, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;


private:
	UPROPERTY(EditAnywhere, Category = "Soul")
	int32 Souls;

	double DesiredZ;

	UPROPERTY(EditAnywhere)
	float DriftRate = -40.f;



public:
	FORCEINLINE int32 GetSouls() const { return Souls;  }
	FORCEINLINE void SetSouls(int32 NumberOfSouls) { Souls = NumberOfSouls; }
};