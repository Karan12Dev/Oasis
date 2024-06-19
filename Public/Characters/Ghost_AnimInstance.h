// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterType.h"
#include "Ghost_AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class UE5_LEARNING_API UGhost_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public: 
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float Deltatime) override;
	
	UPROPERTY(BlueprintReadOnly)
	class AGhost* GhostCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* GhostCharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool IsFalling;

	UPROPERTY(BlueprintReadOnly, Category = "Movement | Character State")
	ECharacterState CharacterState;
};
