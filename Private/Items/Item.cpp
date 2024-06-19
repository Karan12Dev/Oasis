// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "Components/SphereComponent.h"
#include "Characters/Ghost.h"
#include "NiagaraComponent.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponet"));
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = ItemMesh;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereMesh"));
	Sphere->SetupAttachment(GetRootComponent());

	EmbersEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Embers Effect"));
	EmbersEffect->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}

float AItem::TransformedSin()
{
	return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}

float AItem::TransformedCosin()
{
	return Amplitude * FMath::Cos(RunningTime * TimeConstant);
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherCom, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AGhost* Ghost = Cast<AGhost>(OtherActor);
	if (Ghost)
	{
		Ghost->SetOverlappingItem(this);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AGhost* Ghost = Cast<AGhost>(OtherActor);
	if (Ghost)
	{
		Ghost->SetOverlappingItem(nullptr);
	}
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorWorldRotation(FRotator(0.f, RotationSpeed * DeltaTime, 0.f));

	RunningTime += DeltaTime;

	if (ItemState == EItemState::Hovering)
	{
		AddActorWorldOffset(FVector(0.f, 0.f, TransformedSin()));
	}
	
}

