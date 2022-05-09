// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Main.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/EngineTypes.h"
#include "Components/BoxComponent.h"
#include "Enemy.h"


AWeapon::AWeapon()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(AItem::Mesh);

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(WeaponMesh);

	bKeepParticlesOnPickup = false;

	WeaponState = EWeaponState::EWS_Pickup;

	Damage = 25.f;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnCombatOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnCombatOverlapEnd);
}

void AWeapon::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (WeaponState == EWeaponState::EWS_Pickup && OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			Main->SetActiveOverlappingItem(this);
		}
	}
}

void AWeapon::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent*OtherComp, 
	int32 OtherBodyIndex
)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	AMain* Main = Cast<AMain>(OtherActor);
	if (Main)
	{
		Main->SetActiveOverlappingItem(nullptr);
	}
}

void AWeapon::Equip(AMain* Char)
{
	if (Char)
	{
		WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		WeaponMesh->SetSimulatePhysics(false);

		const USkeletalMeshSocket* RightHandSocket = Char->GetMesh()->GetSocketByName("RightHandSocket");
		if (RightHandSocket)
		{
			Mesh->SetWorldRotation(FRotator(0.f));
			bRotate = false;
			if (!bKeepParticlesOnPickup)
			{
				IdleParticlesComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
				IdleParticlesComponent->Deactivate();
			}
			Char->SetEquippedWeapon(this);
			Char->SetActiveOverlappingItem(nullptr);
			RightHandSocket->AttachActor(this, Char->GetMesh());
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Equipped"));
			if (OnEquipSound)
				UGameplayStatics::PlaySound2D(this, OnEquipSound);
		}
	}
}

void AWeapon::OnCombatOverlapBegin(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult
)
{
	if (OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			if (Enemy->HitParticles)
			{
				const USkeletalMeshSocket* HitParticleSocket = WeaponMesh->GetSocketByName("HitParticleSocket");
				if (HitParticleSocket)
				{
					FVector SocketLocation = HitParticleSocket->GetSocketLocation(WeaponMesh);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, GetActorLocation(), FRotator(0.f));
				}
			}
		}
	}
}

void AWeapon::OnCombatOverlapEnd(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* 
	OtherComp, 
	int32 OtherBodyIndex
)
{
}
