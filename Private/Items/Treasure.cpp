// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasure.h"
#include "Characters/Ghost.h"
#include "Kismet/GameplayStatics.h"

void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherCom, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AGhost* Ghost = Cast<AGhost>(OtherActor);
	if (Ghost)
	{
		if (CoinSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, CoinSound, GetActorLocation());
		}
		Destroy();
	}
}
