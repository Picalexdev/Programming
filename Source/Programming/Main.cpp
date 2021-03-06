// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Containers/Array.h"
#include "Weapon.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Sound/SoundCue.h"
#include "Enemy.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AMain::AMain()
{
 	// Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Camera Boom (Pulls toward the player if there's a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent()); // Parent to the root
	CameraBoom->TargetArmLength = 600.f; // Distance from the player
	CameraBoom->bUsePawnControlRotation = true; // Rotation taken from Controller
		// Camera lags behind the character
	CameraBoom->bEnableCameraLag = false;
	CameraBoom->CameraLagSpeed = 12.f;

	// Capsule dimensions and location
	GetCapsuleComponent()->InitCapsuleSize(32.f, 100.f);
	GetCapsuleComponent()->SetRelativeLocation(FVector(0.f, 0.f, 0.f));

	// SkeletalMesh location and rotation
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -102.f), FRotator(0.f, -90.f, 0.f));

	// Create follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to end of boom
	FollowCamera->bUsePawnControlRotation = false;

	// Set turn speed
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	// So the character doesn't turn while looking around
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input
	GetCharacterMovement()->RotationRate = FRotator(0.f, 600.f, 0.f); // Rotation at which the character turns
	GetCharacterMovement()->JumpZVelocity = 550.f; //Jump height
	GetCharacterMovement()->AirControl = 0.2f; // Ability to change direction in the air
	GetCharacterMovement()->MaxAcceleration = 300.f; // Acceleration amount
	GetCharacterMovement()->GroundFriction = 100.f; // Friction with the ground
	GetCharacterMovement()->BrakingFrictionFactor = 0.0001f; // Deceleration multiplier (GroundFriction * BrakingFrictionFactor)

	// Set default stats for the player
	MaxHealth = 100.f;
	Health = 65.f;
	MaxStamina = 150.f;
	Stamina = 105.f;
	Coins = 0.f;

	RunningSpeed = 500.f;
	SprintingSpeed = 750.f;
	bShiftKeyDown = false;

	// Initialize Enums
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	StaminaDrainRate = 25.f;
	MinSprintStamina = 30.f;

	bLmbDown = false;
	bInteractDown = false;

	InterpSpeed = 15.f;
	bInterpToEnemy = false;
}

void AMain::ShowPickupLocations()
{
	for (FVector Location : PickupLocations)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 12, FLinearColor::Green, 10.f, 1.f);
	}
}


float AMain::DecrementHealth(float DamageAmount)
{
	if (Health - DamageAmount <= 0.F)
	{
		float Ret = Health;
		Health = 0.f;
		Die();
		return Ret;
	}
	else
	{
		Health -= DamageAmount;
		return DamageAmount;
	}
}

float AMain::TakeDamage(
	float DamageAmount, 
	FDamageEvent const& DamageEvent, 
	AController* EventInstigator,
	AActor* DamageCauser
)
{
	float Ret = DecrementHealth(DamageAmount);

	return Ret;
}

void AMain::IncrementCoins(int32 Amount)
{
	Coins += Amount;
}

void AMain::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"));
	}
	MovementStatus = EMovementStatus::EMS_Dead;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DeltaStamina = StaminaDrainRate + DeltaTime;

	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
		if (bShiftKeyDown) 
		{
			if (Stamina - DeltaStamina <= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
			}
			Stamina -= DeltaStamina;
			SetMovementStatus(EMovementStatus::EMS_Sprinting);
		}
		else
		{
			if (Stamina + DeltaStamina >= MaxStamina)
			{
				Stamina = MaxStamina;
			}
			else
				Stamina += DeltaStamina;
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;
	case EStaminaStatus::ESS_BelowMinimum:
		if (bShiftKeyDown)
		{
			if (Stamina - DeltaStamina <= 0.f)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				Stamina = 0.f;
				SetMovementStatus(EMovementStatus::EMS_Normal);

			}
			else
			{
				Stamina -= DeltaStamina;
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
		}
		else
		{
			if (Stamina + DeltaStamina >= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
			}
			Stamina += DeltaStamina;
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;
	case EStaminaStatus::ESS_Exhausted:
		if (bShiftKeyDown)
		{
			Stamina = 0.f;
		}
		else
		{
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	case EStaminaStatus::ESS_ExhaustedRecovering:
		if (Stamina + DeltaStamina >= MinSprintStamina)
		{
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
			Stamina += DeltaStamina;
		}
		else
			Stamina += DeltaStamina;
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	default:
		;
	}

	if (bInterpToEnemy && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotator = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(InterpRotator);
	}

}

FRotator AMain::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;
}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	// Movement
	PlayerInputComponent->BindAxis("MoveForward", this, &AMain::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMain::MoveRight);
		// Sprint
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMain::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMain::ShiftKeyUp);
		// Jump
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Attack
	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMain::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMain::LMBUp);

	// Item interaction
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMain::InteractDown);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AMain::InteractUp);

	//Camera
		// Keyboard
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMain::LookUpAtRate);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMain::TurnAtRate);
		// Mouse
	PlayerInputComponent->BindAxis("ViewPitch", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("ViewYaw", this, &APawn::AddControllerYawInput);

}

// MOVEMENT_________________________________________________________________________________
void AMain::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.0f && !bAttacking)
	{
		// Get the direction the camera is looking
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		// RotationMatrix gets world orientation
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMain::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.0f && !bAttacking)
	{
		// Get the direction the camera is looking
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		// RotationMatrix gets world orientation X = Forward Y = Right Z = Up
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}



// Sprinting status
void AMain::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;
	if (MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
}

void AMain::ShiftKeyDown()
{
	bShiftKeyDown = true;
}

void AMain::ShiftKeyUp()
{
	bShiftKeyDown = false;
}


// INPUT____________________________________________________________________________________
void AMain::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMain::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMain::LMBDown()
{

	bLmbDown = true;
	if (EquippedWeapon && !bAttacking)
		Attack();
}

void AMain::LMBUp()
{
	bLmbDown = false;
}

void AMain::InteractDown()
{
	bInteractDown = true;
	if (ActiveOverlappingItem)
	{
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if (Weapon)
		{
			Weapon->Equip(this);
			Weapon->SetWeaponState(EWeaponState::EWS_Equipped);
			SetActiveOverlappingItem(nullptr);
			GetCharacterMovement()->RotationRate = FRotator(0.f, 1200.f, 0.f);

		}
	}
}

void AMain::InteractUp()
{
}

void AMain::SetEquippedWeapon(AWeapon* WeaponToSet)
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
	EquippedWeapon = WeaponToSet;

}

void AMain::Attack()
{

	bAttacking = true;
	SetInterpToEnemy(true);
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		int32 Section = FMath::RandRange(0, 1);
		AnimInstance->Montage_Play(CombatMontage, 2.f);
		switch (Section)
		{
		case 0:
			AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
			break;
		case 1:
			AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
			break;
		default:
			break;
		}
	}
}

void AMain::AttackEnd()
{
	bInterpToEnemy = false;
	bAttacking = false;
}

void AMain::PlaySwingSound()
{
	if (EquippedWeapon)
	{
		if (EquippedWeapon->SwingSound)
			UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
	}
}