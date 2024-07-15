// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterType.h"
#include "BaseCharacter.generated.h"


class ASword;
class UAttributeComponent;
class UAnimMontage;


UCLASS()
class UE5_LEARNING_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;



protected:
	virtual void BeginPlay() override;

	bool IsAlive();
	virtual void Die();
	
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	void PlayHitReactMontage(const FName& SectionName);
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	void DirectionalHitReact(const FVector& ImpactPoint);
	virtual void HandleDamage(float DamageAmount);
	void StopAttackMontage();

	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)
	void SetSwordCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UFUNCTION(BlueprintCallable)
	void SetWeaponsCollsionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();
	
	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();


	UPROPERTY(EditAnywhere, Category = "Weapon")
	TArray<TSubclassOf<class ASword>> WeaponClasses;

	UPROPERTY(EditAnywhere, Category = "Weapon | Socket Name")
	TArray<FName> SocketNames;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	TArray<ASword*> EquippedWeapons;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	ASword* EquippedSword;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	ASword* EquippedDagger;

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category = Combat)
	double WarpTargetDistance = 0.f;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeadPose> DeadPose;

	

private:
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);


	UPROPERTY(EditAnywhere, Category = Sounds)
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = VisualEffects)
	UParticleSystem* HitParticles;

	UPROPERTY(EditDefaultsOnly, Category = AnimationMontages)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montage)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montage)
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> AttackMontageSections;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> DeathMontageSections;



public:
	FORCEINLINE TEnumAsByte<EDeadPose> GetDeadPose() const { return DeadPose;  }
};
