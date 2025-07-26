// Copyright © 2025 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "UObject/NoExportTypes.h"

/**
 * @brief Toggles developer debug mode for custom logging and diagnostics.
 *
 * Set to 1 to enable detailed logging and debugging features intended for development.
 * Set to 0 to disable debug mode for production builds.
*/
#define DEV_DEBUG_MODE 1

// Logs a warning message to the console.
// Usage: LOG_WARNING("Your warning message here.");
#define LOG_WARNING(Message) UE_LOG(LogTemp, Warning, TEXT(Message))

// Define a custom logging macro
// Usage : 
// float MyFloatValue = 3.14f;
// LOG_WARNING_FLOAT("MyFloatValue", MyFloatValue);
#define LOG_WARNING_FLOAT(VariableName, VariableValue) \
 UE_LOG(LogTemp, Warning, TEXT("%s: %f"), TEXT(VariableName), VariableValue)

// Define a custom logging macro for integers
// Usage : 
// int32 MyIntValue = 42;
// LOG_WARNING_INT("MyIntValue", MyIntValue);
#define LOG_WARNING_INT(VariableName, VariableValue) \
 UE_LOG(LogTemp, Warning, TEXT("%s: %d"), TEXT(VariableName), VariableValue)

// Logs a warning message with an FVector value to the console.
// Usage: LOG_WARNING_FVECTOR("VariableName", VariableValue);
#define LOG_WARNING_FVECTOR(VariableName, VariableValue) \
 UE_LOG(LogTemp, Warning, TEXT("%s: %s"), TEXT(VariableName), *VariableValue.ToString())

// Logs a message to the console.
// Usage: LOG("Your log message here.");
#define LOG(Message) UE_LOG(LogTemp, Log, TEXT(Message))

// Logs an error message to the console with additional context including the current function name, 
// line number, and the filename where the error occurred, along with a custom message.
// Usage: LOG_ERROR("Your error message here.");
#define LOG_ERROR(Message) \
UE_LOG(LogTemp, Error, TEXT("%s:%d: %s: %s"), TEXT(__FILE__), __LINE__, TEXT(__FUNCTION__), TEXT(Message))

// Logs an informational message to the console.
// Usage: LOG_INFO("Your informational message here.");
#define LOG_INFO(Message) UE_LOG(LogTemp, Display, TEXT(Message))

// Logs a verbose message to the console.
// Usage: LOG_VERBOSE("Your verbose message here.");
#define LOG_VERBOSE(Message) UE_LOG(LogTemp, Verbose, TEXT(Message))

// Logs a fatal error message to the console and terminates the program.
// Usage: LOG_FATAL("Your fatal error message here.");
#define LOG_FATAL(Message) UE_LOG(LogTemp, Fatal, TEXT("%s (%s:%d): %s"), TEXT(__FUNCTION__), TEXT(__FILE__), __LINE__, TEXT(Message))

// Macro to log a formal fatal error message when the game crashes
#define LOG_FATAL_USER() UE_LOG(LogTemp, Fatal, TEXT("The game has encountered an error and has crashed. We appreciate your assistance in submitting a crash report."))

// Logs a silent message to the console. This is useful for messages that should not appear unless explicitly configured to do so.
// Usage: LOG_SILENT("Your silent message here.");
#define LOG_SILENT(Message) UE_LOG(LogTemp, Log, TEXT(Message))

// This macro logs a message to the screen using GEngine with the specified color.
// Usage: LOG_GENGINE("Your message here", "ColorName");
// Example: LOG_GENGINE("Hello, world!", "Red");
// Supported colors: Red, Green, Blue, Cyan, Magenta, Yellow, White
#define LOG_GENGINE(Message, Color) \
if (GEngine) \
{ \
    FColor LogColor; \
    if (FString(Color) == "Red") \
    { \
        LogColor = FColor::Red; \
    } \
    else if (FString(Color) == "Green") \
    { \
        LogColor = FColor::Green; \
    } \
    else if (FString(Color) == "Blue") \
    { \
        LogColor = FColor::Blue; \
    } \
    else if (FString(Color) == "Cyan") \
    { \
        LogColor = FColor::Cyan; \
    } \
    else if (FString(Color) == "Magenta") \
    { \
        LogColor = FColor::Magenta; \
    } \
    else if (FString(Color) == "Yellow") \
    { \
        LogColor = FColor::Yellow; \
    } \
    else if (FString(Color) == "White") \
    { \
        LogColor = FColor::White; \
    } \
    else \
    { \
        LogColor = FColor::White; \
    } \
    GEngine->AddOnScreenDebugMessage(-1, 15.f, LogColor, FString(Message)); \
}



// Logs an invalid object message to the screen using DiagnosticSystem.
// Usage: LOG_INVALID(YourObject);
// Example: LOG_INVALID(WeaponData);
#define LOG_INVALID(InvalidObjectInput) \
if (GEngine) \
{ \
    DiagnosticSystem::LogInvalid( \
        TEXT(#InvalidObjectInput), \
        TEXT(__FUNCTION__), \
        TEXT(__FILE__), \
        __LINE__ \
    ); \
}

// Logs a TODO message to the screen with custom message, function, file, and line info.
// Usage: LOG_TODO("CustomMessage");
// Example: LOG_TODO("This item is not dismantlable");
#define LOG_TODO(CustomMessage) \
UE_LOG(LogTemp, Warning, TEXT("TODO: %s\nFunction: %s\nFile: %s\nLine: %d"), \
    *FString(CustomMessage), \
    *FString(__FUNCTION__), \
    *FString(__FILE__), \
    __LINE__)

#define LOG_TODO_GENGINE(CustomMessage) \
if (GEngine) \
{ \
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, \
    FString::Printf(TEXT("TODO: %s\nFunction: %s\nFile: %s\nLine: %d"), \
    *FString(CustomMessage), \
    *FString(__FUNCTION__), \
    *FString(__FILE__), \
    __LINE__)); \
}

#define LOG_TO_FILE(Message) \
{ \
    FDateTime CurrentTime = FDateTime::Now(); \
    FString Timestamp = CurrentTime.ToString(); \
    FString FileName = FPaths::GetCleanFilename(__FILE__); \
    FString LogMessage = FString::Printf(TEXT("[%s] %s\nFile: %s\nFunction: %s\nLine: %d\n"), \
        *Timestamp, \
        ANSI_TO_TCHAR(Message), \
        *FileName, \
        ANSI_TO_TCHAR(__FUNCTION__), \
        __LINE__); \
    FString FilePath = TEXT("X:\\Age Of Reverse\\Genesis\\Source\\DeveloperLogs.txt"); \
    FString ExistingContent; \
    FFileHelper::LoadFileToString(ExistingContent, *FilePath); \
    ExistingContent += LogMessage; \
    if (FFileHelper::SaveStringToFile(ExistingContent, *FilePath)) \
    { \
        UE_LOG(LogTemp, Log, TEXT("Logged to file: %s"), *LogMessage); \
    } \
    else \
    { \
        UE_LOG(LogTemp, Error, TEXT("Failed to write to log file: %s"), *FilePath); \
    } \
}

// If DEV_DEBUG_MODE is defined and set to true, enable debug logging for developers as invalid log system.
// Otherwise, use a fatal error logging mechanism for users.
#if defined(DEV_DEBUG_MODE) && DEV_DEBUG_MODE
#define LOG_IF_DEBUG(Object) LOG_INVALID(Object)
#else
#define LOG_IF_DEBUG(Object) LOG_FATAL_USER()
#endif

// A safe check macro to validate if an object is null.
// If the object is null, it logs an error based on the debug mode setting.
// - In debug mode (DEV_DEBUG_MODE enabled), it logs an invalid object message for debugging.
// - In release mode (DEV_DEBUG_MODE disabled), it triggers a fatal error, potentially crashing the application.
#define SAFE_CHECK(Object)      \
if ((Object) == nullptr)        \
{                               \
    LOG_IF_DEBUG(Object);       \
}

#define SAFE_GETTER(Pointer, ReturnType, ContextName, PointerName) \
    if (!(Pointer)) \
    { \
        UE_LOG(LogTemp, Error, TEXT("%s: %s is null! [File: %s, Line: %d, Function: %s] Ensure it is set before accessing."), \
            *ContextName, *PointerName, TEXT(__FILE__), __LINE__, TEXT(__FUNCTION__)); \
        return nullptr; \
    } \
    UE_LOG(LogTemp, Verbose, TEXT("%s: %s retrieved successfully. [File: %s, Line: %d, Function: %s]"), \
        *ContextName, *PointerName, TEXT(__FILE__), __LINE__, TEXT(__FUNCTION__)); \
    return (Pointer);



// A safe check macro to validate multiple objects for null pointers.
// If any object is null, it logs an error based on the debug mode setting.
// - In debug mode (DEV_DEBUG_MODE enabled), it logs an invalid object message for debugging.
// - In release mode (DEV_DEBUG_MODE disabled), it triggers a fatal error, potentially crashing the application.
#define SAFE_CHECK_MULTIPLE(ObjectArray) \
{ \
    for (auto& Object : ObjectArray) \
    { \
        if ((Object) == nullptr) \
        { \
            LOG_IF_DEBUG(Object); \
        } \
    } \
}




template <typename KeyType, typename ValueType>
inline bool AreMapsEqual(const TMap<KeyType, ValueType>& MapA, const TMap<KeyType, ValueType>& MapB)
{
    // If the maps have different sizes, they are not equal
    if (MapA.Num() != MapB.Num())
    {
        return false;
    }

    // Iterate over MapA and check if all key-value pairs exist in MapB
    for (const TPair<KeyType, ValueType>& PairA : MapA)
    {
        // Check if MapB contains the same key and value
        const ValueType* ValueB = MapB.Find(PairA.Key);
        if (!ValueB || *ValueB != PairA.Value)
        {
            return false;
        }
    }

    // If all checks passed, the maps are equal
    return true;
}

class AGEOFREVERSE_API DiagnosticSystem
{

public:
    // Get class name from function name
    static FString GetClassName(const FString& FunctionName);

    // Log invalid object
    static void LogInvalid(const FString& InvalidObjectName, const FString& FunctionName, const FString& FileName, int32 LineNumber);

};
