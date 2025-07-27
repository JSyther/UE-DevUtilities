#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEventLogger.generated.h"

USTRUCT(BlueprintType)
struct FGameplayEventEntry
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString EventName;

    UPROPERTY(BlueprintReadOnly)
    FString Context;

    UPROPERTY(BlueprintReadOnly)
    float GameTime;

    FGameplayEventEntry() {}

    FGameplayEventEntry(const FString& InName, const FString& InContext, float InTime)
    : EventName(InName)
    , Context(InContext)
    , GameTime(InTime)
    {}
};

/**
 * UGameplayEventLogger
 * --------------------
 * Logs gameplay-related events at runtime with timestamps.
 * Useful for QA, debugging, or postmortem analysis.
 */
UCLASS(ClassGroup=(DevTools), meta=(BlueprintSpawnableComponent))
class YOURPROJECT_API UGameplayEventLogger : public UActorComponent
{
    GENERATED_BODY()

public:
    UGameplayEventLogger();

    /** Logs an event with optional context info */
    UFUNCTION(BlueprintCallable, Category="Event Logger")
    void LogEvent(const FString& EventName, const FString& Context = TEXT(""));

    /** Clears the event log */
    UFUNCTION(BlueprintCallable, Category="Event Logger")
    void ClearLog();

    /** Dumps the event log to Output Log */
    UFUNCTION(BlueprintCallable, Category="Event Logger")
    void DumpLogToConsole() const;

    /** Returns the internal array of all logged events */
    UFUNCTION(BlueprintCallable, Category="Event Logger")
    const TArray<FGameplayEventEntry>& GetEventLog() const;

protected:
    UPROPERTY()
    TArray<FGameplayEventEntry> EventLog;
};
