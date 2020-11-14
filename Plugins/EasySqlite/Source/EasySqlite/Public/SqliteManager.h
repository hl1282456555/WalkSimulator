// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "sqlite3.h"
#include "SqliteManager.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EDataBaseValueTypes : uint8
{
	NONE,
	INTEGER,
	REAL,
	TEXT,
	NUMERIC,
};

UCLASS(BlueprintType)
class EASYSQLITE_API USqliteManager : public UObject
{
	GENERATED_BODY()

private:
	static USqliteManager* SqliteManager;

	TMap<FString, sqlite3*> DataBaseTables;

	virtual void BeginDestroy() override;
	
private:
	USqliteManager() {};

public:
	UFUNCTION(BlueprintCallable, Category = "SqliteManager")
	static USqliteManager* GetSqliteManager();

	UFUNCTION(BlueprintCallable, Category = "SqliteManager")
	bool OpenDataBase(FString DataBasePath);

	UFUNCTION(BlueprintCallable, Category = "SqliteManager")
	bool CreateTable(FString DataBasePath, FString TableName, TMap<FString, EDataBaseValueTypes> Data, bool PrimaryKeyAutoIncrement);

	//If value type is text in data base, the value 
	UFUNCTION(BlueprintCallable, Category = "SqliteManager")
	bool InsertItem(FString DataBasePath, FString TableName, TMap<FString, FString> Data);

	void CloseDataBase();
};
