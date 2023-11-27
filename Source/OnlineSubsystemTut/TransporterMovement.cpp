// Fill out your copyright notice in the Description page of Project Settings.


#include "TransporterMovement.h"

#include "PressurePlate.h"


// Sets default values for this component's properties
UTransporterMovement::UTransporterMovement()
{

	// PrimaryComponentTick.bCanEverTick = true;

	// this is the best way of turning on replication for an actor component in the constructor
	SetIsReplicatedByDefault(true);
	
	ActivatedTriggerCount = 0;
	
	
}


// Called when the game starts
void UTransporterMovement::BeginPlay()
{
	Super::BeginPlay();

	for (AActor* TA : TriggerActors)
	{
		if (APressurePlate* PressurePlateActor = Cast<APressurePlate>(TA))
		{
			PressurePlateActor->OnActivated.AddDynamic(this, &UTransporterMovement::OnPressurePlateActivated);
			PressurePlateActor->OnDeactivated.AddDynamic(this, &UTransporterMovement::OnPressurePlateDeactivated);
		}
	}
}

void UTransporterMovement::OnPressurePlateActivated()
{
	ActivatedTriggerCount++;

	FString Msg = FString::Printf(TEXT("Transporter Activated: %d"), ActivatedTriggerCount);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, Msg);

	if (TriggerActors.Num() > 0)
	{
		AllTriggerActorsTriggered = (ActivatedTriggerCount >= TriggerActors.Num());
		if (AllTriggerActorsTriggered)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString("AllTriggerActorsTriggered!"));
			
			if (const AActor* MyOwner = GetOwner())
			{
				if (USceneComponent* RootComponent = MyOwner->GetRootComponent())
				{
					RootComponent->SetMobility(EComponentMobility::Movable);
				}
			}
		}
	}
}

void UTransporterMovement::OnPressurePlateDeactivated()
{
	ActivatedTriggerCount--;

	FString Msg = FString::Printf(TEXT("Transporter Activated: %d"), ActivatedTriggerCount);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, Msg);

	AllTriggerActorsTriggered = (ActivatedTriggerCount >= TriggerActors.Num());
	if (!AllTriggerActorsTriggered)
	{
		if (const AActor* MyOwner = GetOwner())
		{
			if (USceneComponent* RootComponent = MyOwner->GetRootComponent())
			{
				RootComponent->SetMobility(EComponentMobility::Static);
			}
		}
	}
	
}














