#include "AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAttributeComponent::SetBaseValue(FGameplayTag AttributeTag, float Value)
{
	FAttributeValue& AttributeValue = Attributes.FindOrAdd(AttributeTag);
	AttributeValue.BaseValue = Value;
}

float UAttributeComponent::GetValue(FGameplayTag AttributeTag) const
{
	const FAttributeValue* AttributeValue = Attributes.Find(AttributeTag);
	return AttributeValue ? AttributeValue->GetFinalValue() : 0.0f;
}

bool UAttributeComponent::HasAttribute(FGameplayTag AttributeTag) const
{
	return Attributes.Contains(AttributeTag);
}

void UAttributeComponent::ApplyModifier(const FAttributeModifierSpec& Modifier)
{
	FAttributeValue& AttributeValue = Attributes.FindOrAdd(Modifier.AttributeTag);
	AttributeValue.AdditiveModifier += Modifier.AdditiveValue;
	AttributeValue.MultiplicativeModifier *= Modifier.MultiplicativeValue;
}

void UAttributeComponent::ClearModifiers(FGameplayTag AttributeTag)
{
	if (FAttributeValue* AttributeValue = Attributes.Find(AttributeTag))
	{
		AttributeValue->AdditiveModifier = 0.0f;
		AttributeValue->MultiplicativeModifier = 1.0f;
	}
}
