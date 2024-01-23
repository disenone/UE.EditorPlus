// Copyright (c) 2023 - 2024, Disenone (https://github.com/disenone).
// All rights reserved. Licensed under MIT License.

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
