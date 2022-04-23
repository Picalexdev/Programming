// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROGRAMMING_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUSerWidget> HUDOverlayAsett;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUSerWidget* HUDOverlay;
protected:

	virtual void BeginPlay() override;

};
