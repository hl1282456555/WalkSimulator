// Copyright Epic Games, Inc. All Rights Reserved.

#include "EasySqlite.h"

#define LOCTEXT_NAMESPACE "FEasySqliteModule"

void FEasySqliteModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FString filePath = FPaths::Combine(*FPaths::ProjectPluginsDir(), TEXT("EasySqlite/Sqlite/Win64/libs"), TEXT("sqlite3.dll"));
	if (!FPaths::FileExists(filePath))
	{
		UE_LOG(LogEasySqlite, Log, TEXT("Dll Path %s is not valid"), *filePath);
		return;
	}
	UE_LOG(LogEasySqlite, Log, TEXT("Dll Path %s is valid"), *filePath);
	void* DllHandle = FPlatformProcess::GetDllHandle(*filePath);
	if (DllHandle != nullptr)
	{
		UE_LOG(LogEasySqlite, Log, TEXT("Load sqlite3.dll Success!"));
	}
	else
	{
		UE_LOG(LogEasySqlite, Log, TEXT("Load sqlite3.dll failed!"));
	}
}

void FEasySqliteModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	if (SqliteManager != nullptr)
	{
		SqliteManager->CloseDataBase();
	}
}

void FEasySqliteModule::SetSqliteManager(USqliteManager* InSqliteManager)
{
	SqliteManager = InSqliteManager;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEasySqliteModule, EasySqlite)