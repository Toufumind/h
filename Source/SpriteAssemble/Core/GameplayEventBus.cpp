#include "GameplayEventBus.h"

void UGameplayEventBus::QueueEvent(const FGameplayEvent& Event)
{
	QueuedEvents.Add(Event);
}

void UGameplayEventBus::BroadcastImmediate(const FGameplayEvent& Event)
{
	if (FGameplayNativeEvent* Listeners = ListenersByType.Find(Event.Type))
	{
		Listeners->Broadcast(Event);
	}
}

void UGameplayEventBus::FlushQueuedEvents()
{
	TArray<FGameplayEvent> EventsToBroadcast = MoveTemp(QueuedEvents);
	QueuedEvents.Reset();

	for (const FGameplayEvent& Event : EventsToBroadcast)
	{
		BroadcastImmediate(Event);
	}
}

FDelegateHandle UGameplayEventBus::Subscribe(EGameplayEventType Type, const FGameplayNativeEvent::FDelegate& Delegate)
{
	return ListenersByType.FindOrAdd(Type).Add(Delegate);
}

void UGameplayEventBus::Unsubscribe(EGameplayEventType Type, FDelegateHandle Handle)
{
	if (FGameplayNativeEvent* Listeners = ListenersByType.Find(Type))
	{
		Listeners->Remove(Handle);
	}
}
