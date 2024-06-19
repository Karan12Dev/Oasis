// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
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

	UFUNCTION(BlueprintCallable)
	void SetSwordCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

protected:
	virtual void BeginPlay() override;

	virtual void Die();

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	ASword* EquippedSword;

	// Play montage functions
	
	void PlayHitReactMontage(const FName& SectionName);
	void DirectionalHitReact(const FVector& ImpactPoint);
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);
	virtual void HandleDamage(float DamageAmount);
	bool IsAlive();
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	//	Animation Montages
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

	/*	Components	 */
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	

private:
	UPROPERTY(EditAnywhere, Category = Sounds)
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = VisualEffects)
	UParticleSystem* HitParticles;
};
