	// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Perception/PawnSensingComponent.h"
#include "Items/Weapon/Sword.h"
#include "Items/Soul.h"


AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("Healh Bar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensing"));
	PawnSensing->SightRadius = 3000.f;
	PawnSensing->SetPeripheralVisionAngle(50.f);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyState == EEnemyState::Dead) return;

	if (EnemyState > EEnemyState::Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn();

	if (InTargetRange(CombatTarget, AttackRadius))
	{
		EnemyState = EEnemyState::Attacking;
	}
	else if (!InTargetRange(CombatTarget, AttackRadius))
	{
		EnemyState = EEnemyState::Chasing;
		GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
		MoveToTarget(CombatTarget);
	}

	return DamageAmount;
}

void AEnemy::Destroyed()
{
	if (EquippedSword && EquippedDagger)
	{
		EquippedSword->Destroy();
		EquippedDagger->Destroy();
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	if (EnemyState != EEnemyState::Dead)
	{
		if (HealthBarWidget)
		{
			HealthBarWidget->SetVisibility(true);
		}
	}

	GetWorldTimerManager().ClearTimer(PatrolTimer);
	GetWorldTimerManager().ClearTimer(AttackTimer);

	StopAttackMontage();

	if (InTargetRange(CombatTarget, AttackRadius))
	{
		if (ActorHasTag(FName("Dead"))) return;
		StartAttackTimer();
	}
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("Enemy"));

	EnemyController = Cast<AAIController>(GetController());
	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}

	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}

	MoveToTarget(PatrolTarget);

	UWorld* World = GetWorld();
	if (World == nullptr || WeaponClasses.IsEmpty() || SocketNames.IsEmpty()) return;
	if (WeaponClasses.Num() > 0 && SocketNames.Num() > 0)
	{
		int32 SocketCount = 0;
		for (const TSubclassOf<ASword>& Weapon : WeaponClasses)
		{
			if (SocketCount >= SocketNames.Num() || SocketNames[SocketCount].IsNone()) return;
			ASword* SpawnedWeapon = World->SpawnActor<ASword>(Weapon);
			if (SpawnedWeapon)
			{
				SpawnedWeapon->Equip(GetMesh(), SocketNames[SocketCount], this, this);
				EquippedWeapons.Add(SpawnedWeapon);
				SocketCount++;
			}
		}
	}
}

void AEnemy::Die()
{
	Super::Die();

	EnemyState = EEnemyState::Dead;
	GetWorldTimerManager().ClearTimer(AttackTimer);

	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(5.f);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetSwordCollisionEnabled(ECollisionEnabled::NoCollision);
	SetWeaponsCollsionEnabled(ECollisionEnabled::NoCollision);

	SpawnSoul();
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (Attributes && HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

void AEnemy::AttackEnd()
{
	EnemyState = EEnemyState::NoState;
	CheckCombatTarget();
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
	}
}

void AEnemy::CheckCombatTarget()
{
	if (!InTargetRange(CombatTarget, CombatRadius))
	{
		GetWorldTimerManager().ClearTimer(AttackTimer);
		CombatTarget = nullptr;
		if (HealthBarWidget)
		{
			HealthBarWidget->SetVisibility(false);
		}

		//	lose intrest
		if (EnemyState != EEnemyState::Engaged)
		{
			EnemyState = EEnemyState::Patrolling;
			GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
			MoveToTarget(PatrolTarget);
		}
	}
	else if (!InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::Chasing)
	{
		GetWorldTimerManager().ClearTimer(AttackTimer);
		if (EnemyState != EEnemyState::Engaged)
		{
			EnemyState = EEnemyState::Chasing;
			GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
			MoveToTarget(CombatTarget);
		}
	}
	else if (InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::Attacking &&
		EnemyState != EEnemyState::Engaged && EnemyState != EEnemyState::Dead)
	{
		StartAttackTimer();
	}
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::Attack()
{
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead"))) return;

	EnemyState = EEnemyState::Engaged;
	PlayAttackMontage();
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::Attacking;
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr) return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
	EnemyController->MoveTo(MoveRequest);
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}

	if (ValidTargets.Num() > 0)
	{
		const int32 SelectNextPatrolTarget = FMath::RandRange(0, ValidTargets.Num() - 1);
		return ValidTargets[SelectNextPatrolTarget];
	}

	return nullptr;
}

void AEnemy::SpawnSoul()
{
	UWorld* World = GetWorld();
	if (World && SoulClass && Attributes)
	{
		const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 125.f);
		ASoul* SpawnedSoul = World->SpawnActor<ASoul>(SoulClass, SpawnLocation, GetActorRotation());
		if (SpawnedSoul)
		{
			SpawnedSoul->SetSouls(Attributes->GetSouls());
			SpawnedSoul->SetOwner(this);
		}
	}
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	if (EnemyState != EEnemyState::Patrolling) return;

	if (SeenPawn->ActorHasTag(FName("Ghost")))
	{
		GetWorldTimerManager().ClearTimer(PatrolTimer);
		GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
		CombatTarget = SeenPawn;
		
		EnemyState = EEnemyState::Chasing;
		MoveToTarget(CombatTarget);
	}
}