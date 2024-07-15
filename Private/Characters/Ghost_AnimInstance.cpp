// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Ghost_AnimInstance.h"
#include "Characters/Ghost.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UGhost_AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	GhostCharacter = Cast<AGhost>(TryGetPawnOwner());
	if (GhostCharacter)
	{
		GhostCharacterMovement = GhostCharacter->GetCharacterMovement();
	}
}

void UGhost_AnimInstance::NativeUpdateAnimation(float Deltatime)
{
	Super::NativeUpdateAnimation(Deltatime);

	if (GhostCharacterMovement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(GhostCharacterMovement->Velocity);
		IsFalling = GhostCharacterMovement->IsFalling();
		CharacterState = GhostCharacter->GetCharacterState(); 
		ActionState = GhostCharacter->GetActionState();
		DeadPose = GhostCharacter->GetDeadPose();
	}
}
