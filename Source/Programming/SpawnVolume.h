// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class PROGRAMMING_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawner")
	class UBoxComponent* SpawningBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
		TSubclassOf<class ACritter> PawnToSpawn;  
	// Creates a dropdown menu in the (self) section in the blueprint that lets you choose the variable

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Creates a blueprint fuction that can be turned into an event */
	UFUNCTION(BlueprintPure, Category = "Spawner")
	FVector GetSpawnPoint();


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Spawner")
	void SpawnOurPawn(UClass* ToSpawn, const FVector& Location);

};
