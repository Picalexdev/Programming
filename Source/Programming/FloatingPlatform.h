// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingPlatform.generated.h"

UCLASS()
class PROGRAMMING_API AFloatingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingPlatform();

	/** Platform mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Floating Platform")
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	//Creates a visual representation of the vector in the scene so it can be manipulated
	FVector StartPoint;

	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"))
	FVector EndPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Floating Platform")
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Floating Platform")
	float Time;

	FTimerHandle Timer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Floating Platform")
	bool  bInterping;

	float Distance;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ToggleInterping();

	void SwapVectors(FVector& VecOne, FVector& VecTwo);

};
