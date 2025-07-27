#include "MemoryUsageTracker.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "UObject/UObjectIterator.h"
#include "UObject/UnrealType.h"
#include "HAL/PlatformMemory.h"
#include "Misc/OutputDeviceNull.h"

UMemoryUsageTracker::UMemoryUsageTracker()
{
    PrimaryComponentTick.bCanEverTick = true;
    SampleInterval = 5.0f; // default sample interval 5 seconds
}

void UMemoryUsageTracker::BeginPlay()
{
    Super::BeginPlay();
    TimeAccumulator = 0.0f;
}

void UMemoryUsageTracker::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    StopTracking();
}

void UMemoryUsageTracker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (SampleInterval <= 0.f || TrackedObjects.Num() == 0)
    {
        return;
    }

    TimeAccumulator += DeltaTime;
    if (TimeAccumulator >= SampleInterval)
    {
        TimeAccumulator = 0.f;
        ClearCachedInfo();

        for (int32 Index = TrackedObjects.Num() - 1; Index >= 0; --Index)
        {
            if (TWeakObjectPtr<UObject> WeakObj = TrackedObjects[Index])
            {
                UObject* Obj = WeakObj.Get();
                if (Obj && !Obj->IsPendingKill())
                {
                    FMemoryUsageInfo Info;
                    Info.TrackedObject = Obj;
                    Info.ObjectName = Obj->GetName();
                    Info.MemoryBytes = CalculateMemoryUsage(Obj);

                    TSet<UObject*> VisitedSet;
                    Info.NumReferencedObjects = CountReferencedObjects(Obj, VisitedSet);

                    CachedMemoryInfo.Add(Info);
                }
                else
                {
                    // Remove invalid references
                    TrackedObjects.RemoveAtSwap(Index);
                }
            }
            else
            {
                // Remove null weak pointer
                TrackedObjects.RemoveAtSwap(Index);
            }
        }
    }
}

void UMemoryUsageTracker::StartTracking(float SamplingInterval)
{
    SampleInterval = FMath::Max(SamplingInterval, 0.01f);
    SetComponentTickEnabled(true);
}

void UMemoryUsageTracker::StopTracking()
{
    SetComponentTickEnabled(false);
}

void UMemoryUsageTracker::RegisterObject(UObject* ObjectToTrack)
{
    if (!ObjectToTrack)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MemoryUsageTracker] RegisterObject called with null."));
        return;
    }

    for (const TWeakObjectPtr<UObject>& WeakObj : TrackedObjects)
    {
        if (WeakObj.Get() == ObjectToTrack)
        {
            // Already tracked
            return;
        }
    }

    TrackedObjects.Add(ObjectToTrack);
}

void UMemoryUsageTracker::UnregisterObject(UObject* ObjectToRemove)
{
    if (!ObjectToRemove)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MemoryUsageTracker] UnregisterObject called with null."));
        return;
    }

    for (int32 Index = TrackedObjects.Num() - 1; Index >= 0; --Index)
    {
        if (TrackedObjects[Index].Get() == ObjectToRemove)
        {
            TrackedObjects.RemoveAtSwap(Index);
            return;
        }
    }
}

const TArray<FMemoryUsageInfo>& UMemoryUsageTracker::GetTrackedMemoryInfo() const
{
    return CachedMemoryInfo;
}

void UMemoryUsageTracker::DumpMemoryUsageToLog() const
{
    UE_LOG(LogTemp, Log, TEXT("---- Memory Usage Tracker Dump Start ----"));

    for (const FMemoryUsageInfo& Info : CachedMemoryInfo)
    {
        UE_LOG(LogTemp, Log, TEXT("Object: %s | Memory: %.2f KB | References: %d"),
            *Info.ObjectName, Info.MemoryBytes / 1024.0f, Info.NumReferencedObjects);
    }

    UE_LOG(LogTemp, Log, TEXT("---- Memory Usage Tracker Dump End ----"));
}

int64 UMemoryUsageTracker::CalculateMemoryUsage(UObject* Object) const
{
    if (!Object)
    {
        return 0;
    }

    // Rough estimation: sum of all UProperties memory + UObject overhead + referenced subobjects size

    int64 TotalSize = 0;

    // UObject base size (platform dependent, approximate)
    TotalSize += sizeof(*Object);

    // Calculate size of properties
    for (TFieldIterator<FProperty> PropIt(Object->GetClass()); PropIt; ++PropIt)
    {
        FProperty* Property = *PropIt;

        if (!Property)
            continue;

        void* ValuePtr = Property->ContainerPtrToValuePtr<void>(Object);

        // Handle array properties separately
        if (FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property))
        {
            FScriptArrayHelper Helper(ArrayProp, ValuePtr);
            int32 ElementSize = ArrayProp->Inner->ElementSize;

            TotalSize += Helper.Num() * ElementSize;
        }
        else if (FStrProperty* StrProp = CastField<FStrProperty>(Property))
        {
            FString* StrPtr = reinterpret_cast<FString*>(ValuePtr);
            if (StrPtr)
            {
                TotalSize += StrPtr->GetAllocatedSize();
            }
        }
        else if (FNameProperty* NameProp = CastField<FNameProperty>(Property))
        {
            // FName memory handled internally, ignore
        }
        else if (FObjectPropertyBase* ObjProp = CastField<FObjectPropertyBase>(Property))
        {
            // Object references: don't add size here to avoid double counting, handled recursively
        }
        else
        {
            // For simple POD types
            TotalSize += Property->ElementSize;
        }
    }

    return TotalSize;
}

int32 UMemoryUsageTracker::CountReferencedObjects(UObject* Object, TSet<UObject*>& Visited) const
{
    if (!Object || Visited.Contains(Object))
    {
        return 0;
    }

    Visited.Add(Object);

    int32 Count = 1; // count self

    for (TFieldIterator<FProperty> PropIt(Object->GetClass()); PropIt; ++PropIt)
    {
        FProperty* Property = *PropIt;
        if (!Property)
            continue;

        void* ValuePtr = Property->ContainerPtrToValuePtr<void>(Object);

        if (FObjectPropertyBase* ObjProp = CastField<FObjectPropertyBase>(Property))
        {
            UObject* RefObject = ObjProp->GetObjectPropertyValue(ValuePtr);
            if (RefObject && !Visited.Contains(RefObject))
            {
                Count += CountReferencedObjects(RefObject, Visited);
            }
        }
        else if (FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property))
        {
            FScriptArrayHelper Helper(ArrayProp, ValuePtr);
            for (int32 Index = 0; Index < Helper.Num(); ++Index)
            {
                void* ElementPtr = Helper.GetRawPtr(Index);
                if (FObjectPropertyBase* InnerObjProp = CastField<FObjectPropertyBase>(ArrayProp->Inner))
                {
                    UObject* RefObject = InnerObjProp->GetObjectPropertyValue(ElementPtr);
                    if (RefObject && !Visited.Contains(RefObject))
                    {
                        Count += CountReferencedObjects(RefObject, Visited);
                    }
                }
            }
        }
    }

    return Count;
}

void UMemoryUsageTracker::ClearCachedInfo()
{
    CachedMemoryInfo.Empty();
}
