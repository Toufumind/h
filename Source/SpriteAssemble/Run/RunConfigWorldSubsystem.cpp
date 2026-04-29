#include "RunConfigWorldSubsystem.h"
#include "RunConfigDefinition.h"

void URunConfigWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (!ActiveRunConfig && !DefaultRunConfig.IsNull())
	{
		ActiveRunConfig = DefaultRunConfig.LoadSynchronous();
	}
}

void URunConfigWorldSubsystem::SetActiveRunConfig(URunConfigDefinition* InRunConfig)
{
	ActiveRunConfig = InRunConfig;
}

UObject* URunConfigWorldSubsystem::FindConfigObject(FName ConfigKey) const
{
	if (!ActiveRunConfig || ConfigKey.IsNone())
	{
		return nullptr;
	}

	for (const FRunConfigObjectEntry& Entry : ActiveRunConfig->ConfigObjects)
	{
		if (Entry.ConfigKey == ConfigKey)
		{
			return Entry.ConfigObject;
		}
	}

	return nullptr;
}
