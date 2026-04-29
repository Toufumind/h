#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "AttributeTypes.h"
#include "AttributeComponent.generated.h"

UCLASS(ClassGroup=(Gameplay), meta=(BlueprintSpawnableComponent))
class SPRITEASSEMBLE_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAttributeComponent();

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Attribute")
	void SetBaseValue(FGameplayTag AttributeTag, float Value);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Attribute")
	float GetValue(FGameplayTag AttributeTag) const;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Attribute")
	bool HasAttribute(FGameplayTag AttributeTag) const;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Attribute")
	void ApplyModifier(const FAttributeModifierSpec& Modifier);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Attribute")
	void ClearModifiers(FGameplayTag AttributeTag);

private:
	UPROPERTY(EditAnywhere, Category = "Gameplay|Attribute")
	TMap<FGameplayTag, FAttributeValue> Attributes;
};
