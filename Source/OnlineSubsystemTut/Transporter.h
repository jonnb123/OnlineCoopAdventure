#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Transporter.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ONLINESUBSYSTEMTUT_API UTransporter : public UActorComponent
{
	GENERATED_BODY()

public:
	UTransporter();

protected:
	virtual void BeginPlay() override;

public:

	FVector StartPoint;
	FVector EndPoint;
	
	bool ArePointsSet;

	UPROPERTY(EditAnywhere)
	float MoveTime;

	UPROPERTY(EditAnywhere)
	TArray<AActor*> TriggerActors;

	UPROPERTY(VisibleAnywhere)
	int ActivatedTriggerCount;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool AllTriggerActorsTriggered;

	UFUNCTION()
	void SetPoints(FVector Point1, FVector Point2);

	UFUNCTION()
	void OnPressurePlateActivated();

	UFUNCTION()
	void OnPressurePlateDeactivated();
};




