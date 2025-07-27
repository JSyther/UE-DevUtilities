#include "GameplayEventLogger.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/Paths.h"
#include "Engine/Engine.h"

UGameplayEventLogger::UGameplayEventLogger()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UGameplayEventLogger::LogEvent(const FString& EventName, const FString& Context)
{
    if (EventName.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("[GameplayEventLogger] LogEvent called with empty EventName."));
        return;
    }

    float CurrentGameTime = 0.f;
    if (GetWorld())
    {
        CurrentGameTime = GetWorld()->GetTimeSeconds();
    }

    FGameplayEventEntry NewEntry(EventName, Context, CurrentGameTime);

    {
        FScopeLock Lock(&Mutex);
        EventLog.Add(NewEntry);
    }

#if WITH_EDITOR
    UE_LOG(LogTemp, Log, TEXT("[GameplayEventLogger] Event Logged: '%s' | Context: '%s' | GameTime: %.3f | UTC: %s"),
        *EventName, *Context, CurrentGameTime, *NewEntry.RealTimestamp.ToString());
#endif
}

void UGameplayEventLogger::ClearLog()
{
    FScopeLock Lock(&Mutex);
    EventLog.Empty();
}

void UGameplayEventLogger::DumpLogToConsole() const
{
    FScopeLock Lock(&Mutex);

    UE_LOG(LogTemp, Log, TEXT("---- Gameplay Event Log Dump Start ----"));
    for (const FGameplayEventEntry& Entry : EventLog)
    {
        UE_LOG(LogTemp, Log, TEXT("GameTime: %.3f | Event: %s | Context: %s | UTC: %s"),
            Entry.GameTime, *Entry.EventName, *Entry.Context, *Entry.RealTimestamp.ToString());
    }
    UE_LOG(LogTemp, Log, TEXT("---- Gameplay Event Log Dump End ----"));
}

bool UGameplayEventLogger::ExportLogToCSV(const FString& FilePath) const
{
    FScopeLock Lock(&Mutex);

    if (EventLog.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[GameplayEventLogger] No events to export."));
        return false;
    }

    FString CSVContent = TEXT("GameTime,EventName,Context,UTC_Timestamp\n");

    for (const FGameplayEventEntry& Entry : EventLog)
    {
        CSVContent += FString::Printf(
            TEXT("%.3f,%s,%s,%s\n"),
            Entry.GameTime,
            *SanitizeForCSV(Entry.EventName),
            *SanitizeForCSV(Entry.Context),
            *Entry.RealTimestamp.ToString()
        );
    }

    const bool bSaved = FFileHelper::SaveStringToFile(CSVContent, *FilePath);

    if (!bSaved)
    {
        UE_LOG(LogTemp, Error, TEXT("[GameplayEventLogger] Failed to save CSV to path: %s"), *FilePath);
    }

    return bSaved;
}

const TArray<FGameplayEventEntry>& UGameplayEventLogger::GetEventLog() const
{
    return EventLog;
}

TArray<FGameplayEventEntry> UGameplayEventLogger::SearchEventsByName(const FString& SearchTerm) const
{
    FScopeLock Lock(&Mutex);

    TArray<FGameplayEventEntry> Results;
    const FString SearchTermLower = SearchTerm.ToLower();

    for (const FGameplayEventEntry& Entry : EventLog)
    {
        if (Entry.EventName.ToLower().Contains(SearchTermLower))
        {
            Results.Add(Entry);
        }
    }

    return Results;
}

TArray<FGameplayEventEntry> UGameplayEventLogger::SearchEventsByContext(const FString& SearchTerm) const
{
    FScopeLock Lock(&Mutex);

    TArray<FGameplayEventEntry> Results;
    const FString SearchTermLower = SearchTerm.ToLower();

    for (const FGameplayEventEntry& Entry : EventLog)
    {
        if (Entry.Context.ToLower().Contains(SearchTermLower))
        {
            Results.Add(Entry);
        }
    }

    return Results;
}

int32 UGameplayEventLogger::GetEventCount() const
{
    FScopeLock Lock(&Mutex);
    return EventLog.Num();
}

FString UGameplayEventLogger::SanitizeForCSV(const FString& Input) const
{
    FString Output = Input;
    // Escape double quotes by doubling them, and wrap in quotes if contains commas or quotes
    if (Output.Contains(TEXT("\"")) || Output.Contains(TEXT(",")))
    {
        Output.ReplaceInline(TEXT("\""), TEXT("\"\""));
        Output = FString::Printf(TEXT("\"%s\""), *Output);
    }
    return Output;
}
