// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

UCLASS()
class PROGRAMMING_API AMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMain();

	/** Camera boom behind the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Main Camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		float BaseLookUpRate;

	// PLAYER STATS____________________________________________________________________________________

	UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
	float MaxHealth;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float Health;

	UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
	float MaxStamina;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float Stamina;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	int32 Coins;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Call for Input */
	void MoveForward(float Value);
	void MoveRight(float Value);

	/** Change the turn rate 
	* @param Rate: Normalised rate, i.e 1.0 = 100% turn rate
	*/
	void TurnAtRate(float Rate);

	/** Change the look up/down rate
	* @param Rate: Normalised rate, i.e 1.0 = 100% look up/down rate
	*/
	void LookUpAtRate(float Rate);

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE void SetCameraBoom(USpringArmComponent* SpringArm) { CameraBoom = SpringArm; }
};
