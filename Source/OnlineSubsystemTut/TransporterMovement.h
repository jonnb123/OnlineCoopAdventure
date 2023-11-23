// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/InterpToMovementComponent.h"
#include "TransporterMovement.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ONLINESUBSYSTEMTUT_API UTransporterMovement : public UInterpToMovementComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTransporterMovement();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	UPROPERTY(EditAnywhere)
	TArray<AActor*> TriggerActors;

	UPROPERTY(VisibleAnywhere)
	int ActivatedTriggerCount;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool AllTriggerActorsTriggered;

	

	UFUNCTION()
	void OnPressurePlateActivated();

	UFUNCTION()
	void OnPressurePlateDeactivated();
};
