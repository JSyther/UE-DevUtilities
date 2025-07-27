#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEventLogger.generated.h"

USTRUCT(BlueprintType)
struct FGameplayEventEntry
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Event")
    FString EventName;

    UPROPERTY(BlueprintReadOnly, Category = "Event")
    FString Context;

    UPROPERTY(BlueprintReadOnly, Category = "Event")
    float GameTime;

    UPROPERTY(BlueprintReadOnly, Category = "Event")
    FDateTime RealTimestamp;

    FGameplayEventEntry() {}

    FGameplayEventEntry(const FString& InName, const FString& InContext, float InGameTime)
        : EventName(InName), Context(InContext), GameTime(InGameTime), RealTimestamp(FDateTime::UtcNow()) {}
};

/**
 * UGameplayEventLogger
 * --------------------
 * A robust runtime logger component that records gameplay-related events with timestamps,
 * supports filtering, exporting, and querying logs.
 * 
 * Designed for advanced QA, telemetry, debugging, and postmortem analysis.
 */
UCLASS(ClassGroup = (DevTools), meta = (BlueprintSpawnableComponent))
class YOURPROJECT_API UGameplayEventLogger : public UActorComponent
{
    GENERATED_BODY()

public:
    UGameplayEventLogger();

    /** Logs an event with optional context information. Thread-safe. */
    UFUNCTION(BlueprintCallable, Category = "Event Logger")
    void LogEvent(const FString& EventName, const FString& Context = TEXT(""));

    /** Clears the entire event log. */
    UFUNCTION(BlueprintCallable, Category = "Event Logger")
    void ClearLog();

    /** Dumps all logged events to the output log (console). */
    UFUNCTION(BlueprintCallable, Category = "Event Logger")
    void DumpLogToConsole() const;

    /** Exports the event log to a CSV file at the given path. Returns success. */
    UFUNCTION(BlueprintCallable, Category = "Event Logger")
    bool ExportLogToCSV(const FString& FilePath) const;

    /** Returns all logged events as an array. */
    UFUNCTION(BlueprintCallable, Category = "Event Logger")
    const TArray<FGameplayEventEntry>& GetEventLog() const;

    /** Searches events by event name substring (case insensitive). Returns filtered array. */
    UFUNCTION(BlueprintCallable, Category = "Event Logger")
    TArray<FGameplayEventEntry> SearchEventsByName(const FString& SearchTerm) const;

    /** Searches events by context substring (case insensitive). Returns filtered array. */
    UFUNCTION(BlueprintCallable, Category = "Event Logger")
    TArray<FGameplayEventEntry> SearchEventsByContext(const FString& SearchTerm) const;

    /** Returns the count of logged events. */
    UFUNCTION(BlueprintCallable, Category = "Event Logger")
    int32 GetEventCount() const;

protected:
    /** Thread-safe storage of gameplay events. */
    UPROPERTY()
    TArray<FGameplayEventEntry> EventLog;

    mutable FCriticalSection Mutex; // For thread safety

private:
    /** Internal helper to sanitize strings for CSV export. */
    FString SanitizeForCSV(const FString& Input) const;
};
