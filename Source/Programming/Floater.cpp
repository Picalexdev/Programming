// Fill out your copyright notice in the Description page of Project Settings.


#include "Floater.h"

// Sets default values
AFloater::AFloater()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CustomStaticMesh"));


	InitialLocation = FVector(0.0f);
	PlacedLocation = FVector(0.0f);
	WorldOrigin = FVector(0.0f, 0.0f, 0.0f);

	InitialDirecction = FVector(0.0f, 0.0f, 0.0f);

	bInitialisedFloaterLocations = false;
	bShouldFloat = false;

	InitialForce = FVector(200000.f, 0.0f, 0.0f);
	InitialTorque = FVector(200000.f, 0.0f, 0.0f);

	Runningtime = 0;

	A = 0.f;
	B = 0.f;
	C = 0.f;
	D = 0.f;

}

// Called when the game starts or when spawned
void AFloater::BeginPlay()
{
	Super::BeginPlay();

	float InitialX = FMath::FRandRange(-500, 500);
	float InitialY = FMath::FRandRange(-500, 500);
	float InitialZ = FMath::FRandRange(0, 500);

	InitialLocation.X = InitialX;
	InitialLocation.Y = InitialY;
	InitialLocation.Z = InitialZ;

	PlacedLocation = GetActorLocation();

	if (bInitialisedFloaterLocations)
	{
		SetActorLocation(InitialLocation);
	}

	BaseZLocation = PlacedLocation.Z;

	//StaticMesh->AddForce(InitialForce);
	//StaticMesh->AddTorqueInRadians(InitialTorque);
	
}

// Called every frame
void AFloater::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShouldFloat)
	{
		FVector NewLocation = GetActorLocation();

		NewLocation.Z = BaseZLocation + A * FMath::Sin(B * Runningtime - C) + D;

		SetActorLocation(NewLocation);
		Runningtime += DeltaTime;
	}

}

