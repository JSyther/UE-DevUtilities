#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MemoryUsageTracker.generated.h"

/**
 * FMemoryUsageInfo
 * ----------------
 * Struct to hold detailed memory usage information for an actor or component.
 */
USTRUCT(BlueprintType)
struct FMemoryUsageInfo
{
    GENERATED_BODY()

    /** Name of the tracked object (Actor or Component). */
    UPROPERTY(BlueprintReadOnly, Category = "Memory")
    FString ObjectName;

    /** Pointer to the tracked UObject (Actor or Component). */
    UPROPERTY(BlueprintReadOnly, Category = "Memory")
    UObject* TrackedObject = nullptr;

    /** Estimated memory usage in bytes. */
    UPROPERTY(BlueprintReadOnly, Category = "Memory")
    int64 MemoryBytes = 0;

    /** Number of referenced sub-objects (for nested memory tracking). */
    UPROPERTY(BlueprintReadOnly, Category = "Memory")
    int32 NumReferencedObjects = 0;
};

/**
 * UMemoryUsageTracker
 * -------------------
 * Component to monitor and report memory usage of target actors or components.
 * Supports periodic sampling, detailed tracking, and Blueprint integration.
 *
 * Typical usage:
 * - Attach to an Actor (e.g., GameMode, Manager).
 * - Register target actors or components to track.
 * - Receive memory usage reports via Blueprint or logs.
 */
UCLASS(ClassGroup=(DevTools), meta=(BlueprintSpawnableComponent))
class YOURPROJECT_API UMemoryUsageTracker : public UActorComponent
{
    GENERATED_BODY()

public:
    UMemoryUsageTracker();

    /** Starts the memory tracking system with a given sampling interval in seconds. */
    UFUNCTION(BlueprintCallable, Category="Memory Tracker")
    void StartTracking(float SamplingInterval = 5.0f);

    /** Stops the memory tracking system. */
    UFUNCTION(BlueprintCallable, Category="Memory Tracker")
    void StopTracking();

    /** Registers an actor or component to be tracked. */
    UFUNCTION(BlueprintCallable, Category="Memory Tracker")
    void RegisterObject(UObject* ObjectToTrack);

    /** Unregisters an actor or component from tracking. */
    UFUNCTION(BlueprintCallable, Category="Memory Tracker")
    void UnregisterObject(UObject* ObjectToRemove);

    /** Gets current memory usage info for all tracked objects. */
    UFUNCTION(BlueprintCallable, Category="Memory Tracker")
    const TArray<FMemoryUsageInfo>& GetTrackedMemoryInfo() const;

    /** Dumps memory usage info to Output Log for debugging. */
    UFUNCTION(BlueprintCallable, Category="Memory Tracker")
    void DumpMemoryUsageToLog() const;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

private:
    /** Internal timer accumulator for sampling interval. */
    float TimeAccumulator = 0.0f;

    /** Sampling interval in seconds between memory usage scans. */
    UPROPERTY(EditAnywhere, Category="Memory Tracker")
    float SampleInterval = 5.0f;

    /** List of tracked objects (Actors or Components). */
    UPROPERTY()
    TArray<TWeakObjectPtr<UObject>> TrackedObjects;

    /** Cached memory usage results updated at each sampling. */
    UPROPERTY()
    TArray<FMemoryUsageInfo> CachedMemoryInfo;

    /** Performs actual memory measurement for a given UObject. */
    int64 CalculateMemoryUsage(UObject* Object) const;

    /** Helper: Recursively counts referenced objects for memory depth analysis. */
    int32 CountReferencedObjects(UObject* Object, TSet<UObject*>& Visited) const;

    /** Helper: Clears cached memory info. */
    void ClearCachedInfo();
};
