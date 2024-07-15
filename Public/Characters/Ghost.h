// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "CharacterType.h"
#include "BaseCharacter.h"
#include "Interfaces/PickupInterface.h"
#include "Ghost.generated.h"

class UInputMappingContext;
class UInputAction;
class AItem;
class UAnimMontage;
class ASoul;
class ATreasure;


UCLASS()
class UE5_LEARNING_API AGhost : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	AGhost();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Jump() override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddSouls(ASoul* Soul) override;
	virtual void AddGold(ATreasure* Treasure) override;
	virtual void AddHealth(class AHealth* Health) override;


protected:
	virtual void BeginPlay() override;

	void PlayLH_AttackMontage();
	virtual void AttackEnd() override;
	bool CanDisarm();
	bool CanArm();
	void PlayEquipMontage(FName SectionName);
	virtual void Die() override;
	void PlayDodgeMontage();

	UFUNCTION(BluePrintCallable)
	void AttachDagger();

	UFUNCTION(BluePrintCallable)
	void DettachDagger();

	UFUNCTION(BluePrintCallable)
	void FinishEquipping();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();


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

	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* DodgeAction;
	void Dodge(const FInputActionValue& Value);


private:
	void InitializeGhostOverlay(APlayerController* PlayerController);
	void SetHUDHealth();

	UFUNCTION(BlueprintCallable)
	void DodgeEnd();


	UPROPERTY(EditAnywhere, Category = GhostAttachment)
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, Category = GhostAttachment)
	class UCameraComponent* Camera;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	UPROPERTY(VisibleInstanceOnly)
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::Unoccupied;

	UPROPERTY(EditDefaultsOnly, Category = AnimationMontages)
	UAnimMontage* LH_AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = AnimationMontages)
	UAnimMontage* EquipMontage;

	UPROPERTY()
	class UGhostOverlay* GhostOverlay;

	UPROPERTY(EditDefaultsOnly, Category = AnimationMontage)
	UAnimMontage* DodgeMontage;



public: 
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState;  }
};
