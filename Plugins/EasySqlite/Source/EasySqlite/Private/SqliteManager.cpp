// Fill out your copyright notice in the Description page of Project Settings.


#include "SqliteManager.h"
#include "Containers/Map.h"

USqliteManager* USqliteManager::SqliteManager = nullptr;

USqliteManager* USqliteManager::GetSqliteManager()
{
    if (SqliteManager == nullptr)
    {
        SqliteManager = NewObject<USqliteManager>();
		FEasySqliteModule* easySqliteModule = FModuleManager::GetModulePtr<FEasySqliteModule>(FName("EasySqlite"));
		easySqliteModule->SetSqliteManager(SqliteManager);
    }
    return SqliteManager;
}

bool USqliteManager::OpenDataBase(FString DataBasePath)
{
	sqlite3* pDataBase;
	int32 nRes = sqlite3_open(TCHAR_TO_ANSI(*DataBasePath), &pDataBase);
	if (nRes != SQLITE_OK)
	{
		UE_LOG(LogTemp, Warning, TEXT("OPEN DATABASE FAILED!"));
		return false;
	}
	else
	{
		DataBaseTables.Add(DataBasePath, pDataBase);
		UE_LOG(LogTemp, Warning, TEXT("OPEN DATABASE SUCC!"));
		return true;
	}
    return false;
}

bool USqliteManager::CreateTable(FString DataBasePath, FString TableName, TMap<FString, EDataBaseValueTypes> Data, bool PrimaryKeyAutoIncrement)
{
	sqlite3* pTable = DataBaseTables.FindRef(DataBasePath);
	if (pTable == nullptr)
	{
		OpenDataBase(DataBasePath);
		pTable = DataBaseTables.FindRef(DataBasePath);
		if (pTable == nullptr)
		{
			return false;
		}
	}

	FString tableData("");
	//TArray<EDataBaseValueTypes> dataKeys;
	TArray<FString> dataKeys;
	Data.GetKeys(dataKeys);

	UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EDataBaseValueTypes"), true);

	for (int32 index = 0; index < dataKeys.Num(); index++)
	{
		EDataBaseValueTypes currentType = Data.FindRef(dataKeys[index]);
		FString enumType = EnumPtr->GetNameByValue((int)currentType).ToString();
		FString CurDataTypeStr;
		FString lefitStr;
		enumType.Split("::", &lefitStr, &CurDataTypeStr, ESearchCase::IgnoreCase, ESearchDir::FromStart);
		
		tableData += dataKeys[index];
		tableData += " ";
		tableData += CurDataTypeStr;
		
		if (index == 0)
		{
			tableData += " ";
			tableData += "primary key autoincrement";
		}

		if (index != (dataKeys.Num() - 1))
		{
			tableData += ",";
		}
	}

	FString sql = FString::Printf(TEXT("create table %s(%s);"), *TableName, *tableData);

	char* cErrMessage;
	int32 nRes = sqlite3_exec(pTable, TCHAR_TO_ANSI(*sql), 0, 0, &cErrMessage);
	if (nRes != SQLITE_OK)
	{
		FString str(cErrMessage);
		UE_LOG(LogTemp, Warning, TEXT("CREATE TABLE FAILED!"));
		return false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CREATE TABLE SUCCESS!"));
		return true;
	}
}

bool USqliteManager::InsertItem(FString DataBasePath, FString TableName, TMap<FString, FString> Data)
{
    sqlite3* pTable = DataBaseTables.FindRef(DataBasePath);
    if (pTable == nullptr)
    {
		OpenDataBase(DataBasePath);
		pTable = DataBaseTables.FindRef(DataBasePath);
		if (pTable == nullptr)
		{
			return false;
		}
    }

    FString keyNames("");
    FString values("");
	TArray<FString> dataKeys;
	Data.GenerateKeyArray(dataKeys);
	for (int32 index = 0; index < dataKeys.Num(); index++)
	{
		keyNames += dataKeys[index];
		values += Data.FindRef(dataKeys[index]);

		if (index != dataKeys.Num() - 1) 
		{
			keyNames += ",";
			values += ",";
		}
	}

    FString sql = FString::Printf(TEXT("insert into %s(%s) values(%s)"), *TableName, *keyNames, *values);

	char* cErrMessage;
	int32 nRes = sqlite3_exec(pTable, TCHAR_TO_UTF8(*sql), 0, 0, &cErrMessage);
	if (nRes != SQLITE_OK)
	{
		FString str(cErrMessage);
		UE_LOG(LogTemp, Warning, TEXT("INSERT FAILED!"));
		return false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("INSERT SUCCESS!"));
		return true;
	}
}

void USqliteManager::CloseDataBase()
{
	TArray<sqlite3*> dataBaseList;
	DataBaseTables.GenerateValueArray(dataBaseList);
	for (auto database : dataBaseList)
	{
		sqlite3_close(database);
	}
}
