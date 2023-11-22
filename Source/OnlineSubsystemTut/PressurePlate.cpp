// Fill out your copyright notice in the Description page of Project Settings.


#include "PressurePlate.h"


// Sets default values
APressurePlate::APressurePlate()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// replicate for the actor
	bReplicates = true;
	SetReplicateMovement(true);

	Activated = false;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	TriggerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TriggerMesh"));
	TriggerMesh->SetupAttachment(RootComp);
	TriggerMesh->SetIsReplicated(true);
	const auto TriggerMeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (TriggerMeshAsset.Succeeded())
	{
		TriggerMesh->SetStaticMesh(TriggerMeshAsset.Object);
		TriggerMesh->SetRelativeScale3D(FVector(3.5f,3.5f,0.2f));
		TriggerMesh->SetRelativeLocation(FVector(0.f,0.f,30.f));
	}
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComp);
	Mesh->SetIsReplicated(true);
	const auto MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Game/Asian_Village/meshes/props/SM_statue_platform_01.SM_statue_platform_01"));
	if (MeshAsset.Succeeded())
	{
		Mesh->SetStaticMesh(MeshAsset.Object);
		Mesh->SetRelativeScale3D(FVector(1.f,1.f,0.1f));
		Mesh->SetRelativeLocation(FVector(0.f,0.f,7.2f));
	}
}

// Called when the game starts or when spawned
void APressurePlate::BeginPlay()
{
	Super::BeginPlay();

	// the cylinder will be invisible when the game starts
	TriggerMesh->SetVisibility(false);
	TriggerMesh->SetCollisionProfileName(FName("OverlapAll"));
	TriggerMesh->OnComponentBeginOverlap.AddDynamic(this, &APressurePlate::OnTriggerMeshBeginOverlap);
	TriggerMesh->OnComponentEndOverlap.AddDynamic(this, &APressurePlate::OnTriggerMeshEndOverlap);
}

// Called every frame
void APressurePlate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APressurePlate::OnTriggerMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority() && OtherActor->ActorHasTag("TriggerActor"))
	{
		TArray<AActor*> OverlappingActors;
		TriggerMesh->GetOverlappingActors(OverlappingActors);

		if (OverlappingActors.Num() == 1)
		{
			Activated = true;
			// GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, TEXT("Activated"));
			OnActivated.Broadcast();
		}
	}
}

void APressurePlate::OnTriggerMeshEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HasAuthority() && OtherActor->ActorHasTag("TriggerActor"))
	{
		TArray<AActor*> OverlappingActors;
		TriggerMesh->GetOverlappingActors(OverlappingActors);

		if (OverlappingActors.Num() == 0)
		{
			Activated = false;
			// GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, TEXT("Deactivated"));
			OnDeactivated.Broadcast();
		}
	}
}


