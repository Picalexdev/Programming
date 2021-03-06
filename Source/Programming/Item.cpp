// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"
#include "Main.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Collision sphere that will be the trigger for the interaction
	CollisionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionVolume"));
	RootComponent = CollisionVolume;
	// Collision setup
	CollisionVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // Enable query only (no physics interaction)
	CollisionVolume->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic); // Set the object type to static
	CollisionVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore); // Ignore all channels
	// Set it to overlap with pawns
	CollisionVolume->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// Main mesh of the item
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());

	// Main particle system
	IdleParticlesComponent = CreateDefaultSubobject< UParticleSystemComponent>(TEXT("IdleParticlesComponent"));
	IdleParticlesComponent->SetupAttachment(GetRootComponent());

	bRotate = false;
	RotationRate = 45.f;
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	// Set the OnOverlapBegin and OnOverlapEnd functions to fire when needed
	CollisionVolume->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnOverlapBegin);
	CollisionVolume->OnComponentEndOverlap.AddDynamic(this, &AItem::OnOverlapEnd);
	
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bRotate)
	{
		FRotator Rotation = Mesh->GetComponentRotation();
		Rotation.Yaw += DeltaTime * RotationRate;
		Mesh->SetWorldRotation(Rotation);
	}
}

// _________________________________________________________________________________

// Universal OverlapBegin behaviour
void AItem::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap Begin"));
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Overlap Begin"));

	// Spawn particle emitter
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			if (OverlapParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation(), FRotator(0.f), true);
			}
			if (OverlapSound)
			{
				UGameplayStatics::PlaySound2D(this, OverlapSound);
			}
		}
	}
}

// Universal OverlapEnd behaviour
void AItem::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex
)
{
	if (OtherActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlap End"));
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Overlap End"));
	}
}

