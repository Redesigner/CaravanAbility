// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBaseAttributeSet.h"

#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

UCharacterBaseAttributeSet::UCharacterBaseAttributeSet()
{
	MovementSpeed.SetBaseValue(300.0f);
	MovementSpeed.SetCurrentValue(300.0f);

	CurrentHealth.SetBaseValue(100.0f);
	CurrentHealth.SetCurrentValue(100.0f);
}

void UCharacterBaseAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacterBaseAttributeSet, CurrentHealth, COND_None, REPNOTIFY_Always);
}

void UCharacterBaseAttributeSet::OnRep_CurrentHealth(const FGameplayAttributeData& OldCurrentHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterBaseAttributeSet, CurrentHealth, OldCurrentHealth);
}

