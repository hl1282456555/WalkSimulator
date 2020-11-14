// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SqliteManager.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_CLASS(LogEasySqlite, Log, All);

class FEasySqliteModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void SetSqliteManager(USqliteManager* InSqliteManager);

private:
	UPROPERTY()
	USqliteManager* SqliteManager;
};
