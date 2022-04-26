// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"
#include "Main.h"

APickup::APickup()
{
	Count = 1;
}



void APickup::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex,
	bool bFromSweep, 
	const FHitResult& SweepResult
)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	UE_LOG(LogTemp, Warning, TEXT("Pickup Overlap Begin"));
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Pickup Overlap Begin"));

	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			Main->IncrementCoins(Count);
			Main->PickupLocations.Add(GetActorLocation());
			Destroy();
		}
	}
}

void APickup::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor,
	UPrimitiveComponent* 
	OtherComp,
	int32 OtherBodyIndex
)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	UE_LOG(LogTemp, Warning, TEXT("Pickup Overlap End"));
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Pickup Overlap End"));
}

