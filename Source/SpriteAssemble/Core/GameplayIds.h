#pragma once

#include "CoreMinimal.h"
#include "GameplayIds.generated.h"

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FShotId
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 Index = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly)
	int32 Generation = 0;

	bool IsValid() const { return Index != INDEX_NONE; }
	void Reset()
	{
		Index = INDEX_NONE;
		Generation = 0;
	}

	friend bool operator==(const FShotId& A, const FShotId& B)
	{
		return A.Index == B.Index && A.Generation == B.Generation;
	}
};

FORCEINLINE uint32 GetTypeHash(const FShotId& Id)
{
	return HashCombine(GetTypeHash(Id.Index), GetTypeHash(Id.Generation));
}

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FGemRuntimeId
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FGuid Value;

	static FGemRuntimeId NewId()
	{
		FGemRuntimeId Id;
		Id.Value = FGuid::NewGuid();
		return Id;
	}

	bool IsValid() const { return Value.IsValid(); }

	friend bool operator==(const FGemRuntimeId& A, const FGemRuntimeId& B)
	{
		return A.Value == B.Value;
	}
};

FORCEINLINE uint32 GetTypeHash(const FGemRuntimeId& Id)
{
	return GetTypeHash(Id.Value);
}

USTRUCT(BlueprintType)
struct SPRITEASSEMBLE_API FRunNodeId
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FGuid Value;

	static FRunNodeId NewId()
	{
		FRunNodeId Id;
		Id.Value = FGuid::NewGuid();
		return Id;
	}

	bool IsValid() const { return Value.IsValid(); }

	friend bool operator==(const FRunNodeId& A, const FRunNodeId& B)
	{
		return A.Value == B.Value;
	}
};

FORCEINLINE uint32 GetTypeHash(const FRunNodeId& Id)
{
	return GetTypeHash(Id.Value);
}
