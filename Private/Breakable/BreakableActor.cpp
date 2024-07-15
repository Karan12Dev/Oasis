// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Items/Treasure.h" 
#include "Components/CapsuleComponent.h"

// Sets default values
ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("Geometry Collection"));
	SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->bUseSizeSpecificDamageThreshold = true;
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capusule"));
	Capsule->SetupAttachment(GetRootComponent());
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}


void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
	GeometryCollection->OnChaosBreakEvent.AddDynamic(this, &ABreakableActor::ChaosBreak);
}

void ABreakableActor::ChaosBreak(const FChaosBreakEvent& BreakEvent)
{
	GeometryCollection->SetNotifyBreaks(true);
	this -> SetLifeSpan(3.f);
}


void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (bBroken) return;
	bBroken = true;

	if (PotBreakSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PotBreakSound, ImpactPoint);
	}

	UWorld* World = GetWorld();
	if (World && TreasureClasses.Num() > 0)
	{
		const int32 Selection = FMath::RandRange(0, TreasureClasses.Num() - 1);	
		World->SpawnActor<ATreasure>(TreasureClasses[Selection], GetActorTransform());
	}

	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
}