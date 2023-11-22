#include "Transporter.h"

#include "PressurePlate.h"


// Sets default values for this component's properties
UTransporter::UTransporter()
{
	// PrimaryComponentTick.bCanEverTick = true;

	// this is the best way of turning on replication for an actor component in the constructor
	SetIsReplicatedByDefault(true);

	MoveTime = 3.f;
	ActivatedTriggerCount = 0;

	ArePointsSet = false;
	StartPoint = FVector::Zero();
	EndPoint = FVector::Zero();
}


// Called when the game starts
void UTransporter::BeginPlay()
{
	Super::BeginPlay();

	for (AActor* TA : TriggerActors)
	{
		if (APressurePlate* PressurePlateActor = Cast<APressurePlate>(TA))
		{
			PressurePlateActor->OnActivated.AddDynamic(this, &UTransporter::OnPressurePlateActivated);
			PressurePlateActor->OnDeactivated.AddDynamic(this, &UTransporter::OnPressurePlateDeactivated);
		}
	}
}

void UTransporter::OnPressurePlateActivated()
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
		}
	}
	
}

void UTransporter::OnPressurePlateDeactivated()
{
	ActivatedTriggerCount--;

	FString Msg = FString::Printf(TEXT("Transporter Activated: %d"), ActivatedTriggerCount);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, Msg);
}



void UTransporter::SetPoints(FVector Point1, FVector Point2)
{
	if (Point1.Equals(Point2)) return;
	
	StartPoint = Point1;
	EndPoint = Point2;
	ArePointsSet = true;
}











