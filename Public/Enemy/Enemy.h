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

	// <AActor>
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;
	//</AActor>

	// IHitInterface
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;



protected:
	virtual void BeginPlay() override;

	virtual void Die() override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual int32 PlayDeathMontage() override;
	virtual void AttackEnd() override;


	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	EEnemyState EnemyState = EEnemyState::Patrolling;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeadPose> DeadPose;



private:
	//	AI Behavior
	void CheckPatrolTarget();
	void CheckCombatTarget();
	void PatrolTimerFinished();
	void Attack();
	void StartAttackTimer();
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);


	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	class UPawnSensingComponent* PawnSensing;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ASword> WeaponClass;

	UPROPERTY()
	class AAIController* EnemyController;

	UPROPERTY()
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	FTimerHandle PatrolTimer;

	float PatrolWaitMin = 4.f;
	float PatrolWaitMax = 8.f;

	FTimerHandle AttackTimer;

	float AttackMin = 0.5f;
	float AttackMax = 1.f;
};
