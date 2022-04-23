// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingPlatform.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"



// Sets default values
AFloatingPlatform::AFloatingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	StartPoint = FVector(0.f);
	EndPoint = FVector(0.f);

	Speed = 4.f;
	Time = 2.f;

	bInterping = false;

}

// Called when the game starts or when spawned
void AFloatingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	StartPoint = GetActorLocation();
	EndPoint += StartPoint;

	bInterping = false;

	GetWorldTimerManager().SetTimer(Timer, this, &AFloatingPlatform::ToggleInterping, Time);


	Distance = (EndPoint - StartPoint).Size();
}

// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bInterping)
	{
		FVector CurrentLocation = GetActorLocation();
		FVector Interp = FMath::VInterpTo(CurrentLocation, EndPoint, DeltaTime, Speed); // Moves from Current to End at Speed
		SetActorLocation(Interp); // Sets location for the entire actor

		float DistanceTravelled = (GetActorLocation() - StartPoint).Size();
		if (Distance - DistanceTravelled <= 1.f)
		{
			ToggleInterping();
			
			GetWorldTimerManager().SetTimer(Timer, this, &AFloatingPlatform::ToggleInterping, Time);
			SwapVectors(StartPoint, EndPoint);
		}
	}
}

void AFloatingPlatform::ToggleInterping()
{
	bInterping = !bInterping;
}

void AFloatingPlatform::SwapVectors(FVector& VecOne, FVector& VecTwo)
{
	FVector Temp = VecOne;
	VecOne = VecTwo;
	VecTwo = Temp;
}







