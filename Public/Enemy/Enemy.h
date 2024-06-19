// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterType.h"
#include "Characters/BaseCharacter.h"
#include "Enemy.generated.h"


class UHealthBarComponent;


UCLASS()
class UE5_LEARNING_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;
	void CheckPatrolTarget();
	void CheckCombatTarget();
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;
	virtual void Die() override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual int32 PlayDeathMontage() override;
	virtual void AttackEnd() override;

	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();
	void Attack();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	EEnemyState EnemyState = EEnemyState::Patrolling;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeadPose> DeadPose;


private:

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	class UPawnSensingComponent* PawnSensing;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ASword> WeaponClass;

	UPROPERTY()
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;

	/*	Navigation	*/

	// Current PatrolTarget
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	UPROPERTY()
	class AAIController* EnemyController;

	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();

	float WaitMin = 4.f;
	float WaitMax = 8.f;

	FTimerHandle AttackTimer;
	void StartAttackTimer();

	float AttackMin = 0.5f;
	float AttackMax = 1.f;


public:	
	
};
