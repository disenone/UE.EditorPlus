// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FEditorPlusToolsImpl;

class FEditorPlusToolsModule : public IModuleInterface
{
public:

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static FEditorPlusToolsModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FEditorPlusToolsModule>(GetName());
	}

	static bool InAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded(GetName());
	}

	static FName GetName()
	{
		return "EditorPlusTools";
	}
	
private:
	TSharedPtr<FEditorPlusToolsImpl> Impl;
};
