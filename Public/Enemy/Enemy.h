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
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;



protected:
	virtual void BeginPlay() override;

	virtual void Die() override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual void AttackEnd() override;


	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	EEnemyState EnemyState = EEnemyState::Patrolling;



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
	void SpawnSoul();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);


	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	class UPawnSensingComponent* PawnSensing;

	UPROPERTY()
	class AAIController* EnemyController;

	UPROPERTY(EditAnywhere, Category = Combat)
	double CombatRadius = 1000.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	double AttackRadius = 150.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	double AcceptanceRadius = 50.f;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	double PatrolRadius = 200.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrollingSpeed;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float ChasingSpeed;

	FTimerHandle PatrolTimer;

	float PatrolWaitMin = 4.f;
	float PatrolWaitMax = 8.f;

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMin = 0.1f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMax = 0.4f;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<class ASoul> SoulClass;
};
