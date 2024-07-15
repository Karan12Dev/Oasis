// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Sword.generated.h"


class USoundBase;
class UBoxComponent;


/**
 * 
 */
UCLASS()
class UE5_LEARNING_API ASword : public AItem
{
	GENERATED_BODY()
	
public:
	ASword();

	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

	TArray<AActor*> IgnoreActors;


protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherCom, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);



private:
	void BoxTrace(FHitResult& BoxHit);


	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bShowBoxDebug = false;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties")
	USoundBase* EquipSound; 

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UBoxComponent* SwordBox;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;
	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties")
	float Damage = 20.f;


public:
	FORCEINLINE UBoxComponent* GetSwordBox() const { return SwordBox; }

};
