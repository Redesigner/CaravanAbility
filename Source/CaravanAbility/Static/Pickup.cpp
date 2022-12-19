// Copyright 2022 Stephen Melnick


#include "Pickup.h"

// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FGameplayInteraction APickup::HandleInteraction(const FGameplayInteraction& Interaction)
{
	if (Interaction.InteractionType == EGameplayInteractionType::None)
	{
		// Destroy();
		OnPickup();
		return FGameplayInteraction(this, EGameplayInteractionType::Give);
	}
	return FGameplayInteraction::None();
}

