// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TransporterMovement.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Actor.h"
#include "MovableActor.generated.h"

UCLASS()
class ONLINESUBSYSTEMTUT_API AMovableActor : public AActor
{
	GENERATED_BODY()

public:
	AMovableActor();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	USceneComponent* RootComp;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UTransporterMovement*  Transporter;
	
};
