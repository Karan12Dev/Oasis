// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon/Sword.h"
#include "Characters/Ghost.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"


ASword::ASword()
{
	SwordBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Sword Box"));
	SwordBox->SetupAttachment(GetRootComponent());
	SwordBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SwordBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	SwordBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	
	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}


void ASword::BeginPlay()
{
	Super::BeginPlay();

	SwordBox->OnComponentBeginOverlap.AddDynamic(this, &ASword::OnBoxOverlap);
}


void ASword::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);

	AttachMeshToSocket(InParent, InSocketName);
	ItemState = EItemState::Equipped;
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
		this, EquipSound, GetActorLocation()
		);
	}
	if (Sphere)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (EmbersEffect)
	{
		EmbersEffect->Deactivate();
	}
}

void ASword::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}


void ASword::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherCom, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherCom, OtherBodyIndex, bFromSweep, SweepResult);
}

void ASword::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}


void ASword::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherCom, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	FHitResult BoxHit;

	UKismetSystemLibrary::BoxTraceSingle(
		this, 
		Start, 
		End, 
		FVector(7.f, 2.f, 5.f),
		BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		BoxHit,
		true
		);

	if (BoxHit.GetActor())
	{
		UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());

		IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
		if (HitInterface)
		{
			HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint);
		}
		IgnoreActors.AddUnique(BoxHit.GetActor());
		CreateFields(BoxHit.ImpactPoint);
	}
}
