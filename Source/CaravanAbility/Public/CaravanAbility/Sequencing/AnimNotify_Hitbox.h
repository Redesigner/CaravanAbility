// Copyright (c) 2023 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CaravanAbility/GameplayAbilities/Components/ArmorComponent.h"

#include "AnimNotify_Hitbox.generated.h"

/**
 * 
 */
UCLASS()
class CARAVANABILITY_API UAnimNotify_Hitbox : public UAnimNotifyState
{
	GENERATED_BODY()

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;



	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	FName HitboxName = TEXT("New hitbox");

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (MakeEditWidget, AllowPrivateAccess = true))
	FArmorInstance ArmorInstance;

public:
	UPROPERTY()
	class UShapeComponent* VisualizerShape;
};
