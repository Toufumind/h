#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayEvent.h"
#include "GameplayEventBus.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FGameplayNativeEvent, const FGameplayEvent&);

UCLASS()
class SPRITEASSEMBLE_API UGameplayEventBus : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void QueueEvent(const FGameplayEvent& Event);
	void BroadcastImmediate(const FGameplayEvent& Event);
	void FlushQueuedEvents();

	FDelegateHandle Subscribe(EGameplayEventType Type, const FGameplayNativeEvent::FDelegate& Delegate);
	void Unsubscribe(EGameplayEventType Type, FDelegateHandle Handle);

private:
	TArray<FGameplayEvent> QueuedEvents;
	TMap<EGameplayEventType, FGameplayNativeEvent> ListenersByType;
};
