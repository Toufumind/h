#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "IceMirrorDefinition.generated.h"

UCLASS(BlueprintType)
class SPRITEASSEMBLE_API UIceMirrorDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IceMirror")
	float BaseDamage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IceMirror")
	float AttackInterval = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IceMirror")
	float Range = 900.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IceMirror")
	float ShotSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IceMirror")
	float ShotRadius = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IceMirror")
	float ShotLifetime = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IceMirror")
	FName ShotVisualType = TEXT("Default");
};
