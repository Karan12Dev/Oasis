// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "CharacterType.h"
#include "BaseCharacter.h"
#include "Ghost.generated.h"

class UInputMappingContext;
class UInputAction;
class AItem;
class UAnimMontage;


UCLASS()
class UE5_LEARNING_API AGhost : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AGhost();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Jump() override;



protected:
	virtual void BeginPlay() override;

	void PlayLH_AttackMontage();
	virtual void AttackEnd() override;
	bool CanDisarm();
	bool CanArm();
	void PlayEquipMontage(FName SectionName);

	UFUNCTION(BluePrintCallable)
	void AttachDagger();

	UFUNCTION(BluePrintCallable)
	void DettachDagger();

	UFUNCTION(BluePrintCallable)
	void FinishEquipping();


	UPROPERTY(EditAnyWhere, Category = Input)
	UInputMappingContext* GhostContext;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* MovementAction;
	void Move(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LookAction;
	void Look(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* RunAction;
	void Run(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* EquipAction;
	void EquipKeyPressed(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* LMBAction;
	void Attack(const FInputActionValue& Value);



private:
	UPROPERTY(EditAnywhere, Category = GhostAttachment)
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, Category = GhostAttachment)
	class UCameraComponent* Camera;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	UPROPERTY(VisibleInstanceOnly)
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	ASword* EquippedDagger;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::Unoccupied;

	UPROPERTY(EditDefaultsOnly, Category = AnimationMontages)
	UAnimMontage* LH_AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = AnimationMontages)
	UAnimMontage* EquipMontage;



public: 
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }

	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
};
