﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeAbility.h"

#include "AbilitySystemComponent.h"
#include "Tasks/PlayMontageTask.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "CaravanAbility/Character/CharacterBase.h"
#include "CaravanAbility/Character/CharacterBaseMovementComponent.h"

UMeleeAbility::UMeleeAbility()
{
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnlyTermination;
	bReplicateInputDirectly = true;
	bRetriggerInstancedAbility = true;
}


void UMeleeAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ExecuteAttack(Handle, ActorInfo, ActivationInfo);
}


void UMeleeAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


void UMeleeAbility::CommitExecute(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::CommitExecute(Handle, ActorInfo, ActivationInfo);
}


void UMeleeAbility::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	ExecuteAttack(Handle, ActorInfo, ActivationInfo, false);
}


void UMeleeAbility::ExecuteAttack(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool InitialAttack)
{
	// If we don't have an animation, this ability won't work...
	if (!MeleeAnimation)
	{
		return;
	}
	if (InitialAttack)
	{
		CurrentComboState = InitialComboState;
		UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("AnimationMontageTask"), MeleeAnimation, 1.0f, CurrentComboState, false);
		// Make sure to call the end ability function at the end of the ability, or cooldowns etc. won't work
		Task->OnCompleted.AddDynamic(this, &UMeleeAbility::EndAbilityManual);
		Task->ReadyForActivation();

		// Bind our hit functions to the hitbox controller
		if (UActorComponent* ActorComponent = ActorInfo->AvatarActor->GetComponentByClass(UHitboxController::StaticClass()))
		{
			UHitboxController* HitboxController = Cast<UHitboxController>(ActorComponent);
			if (HasAuthority(&ActivationInfo))
			{
				HitboxController->HitDetectedDelegate.BindUObject(this, &UMeleeAbility::HitTarget);
			}
			else
			{
				// Even if we don't have authority, still call the blueprint hit function, so we can apply any cues we need
				HitboxController->HitDetectedDelegate.BindUObject(this, &UMeleeAbility::HitTargetLocal);
			}
			HitboxController->ClearOverlaps();
		}
	}
	else
	{
		FName PreviousComboState = CurrentComboState;
		if (ACharacterBase* CharacterBase = Cast<ACharacterBase>(ActorInfo->AvatarActor))
		{
			CurrentComboState = CharacterBase->GetComboState();
		}
		if (MeleeAnimation->IsValidSectionName(CurrentComboState))
		{
			MontageSetNextSectionName(PreviousComboState, CurrentComboState);
			UE_LOG(LogAbilitySystem, Display, TEXT("Setting montage section from %s to %s"), *PreviousComboState.ToString(), * CurrentComboState.ToString());
		}
	}
}


void UMeleeAbility::EndAbilityManual()
{
	UE_LOG(LogAbilitySystem, Display, TEXT("Ability ended Manually."));
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}


void UMeleeAbility::HitTarget(const FHitResult& HitResult)
{
	OnTargetHit(HitResult);
	int SectionIndex = 0;

	if (UActorComponent* Component = GetCurrentActorInfo()->OwnerActor->GetComponentByClass(USkeletalMeshComponent::StaticClass()))
	{
		USkeletalMeshComponent* SkeletalMeshComp = Cast<USkeletalMeshComponent>(Component);
		SectionIndex = MeleeAnimation->GetSectionIndex(SkeletalMeshComp->GetAnimInstance()->Montage_GetCurrentSection());
	}
	
	if (HitEffects.IsValidIndex(SectionIndex))
	{
		const FGameplayEffectSpecHandle HitEffectSpec = MakeOutgoingGameplayEffectSpec(HitEffects[SectionIndex]);
		FGameplayAbilityTargetDataHandle TargetDataHandle;
	
		TargetDataHandle.Add(new FGameplayAbilityTargetData_SingleTargetHit(HitResult));
		
		ApplyGameplayEffectSpecToTarget(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(),
			HitEffectSpec, TargetDataHandle);
	}
}

void UMeleeAbility::HitTargetLocal(const FHitResult& HitResult)
{
	OnTargetHit(HitResult);
}
