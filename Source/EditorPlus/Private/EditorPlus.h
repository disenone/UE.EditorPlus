// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FEditorPlusMenuManager;

class FEditorPlusModule : public IModuleInterface
{
public:

	virtual bool SupportsDynamicReloading() override { return false; }
	
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static FEditorPlusModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FEditorPlusModule>(GetName());
	}

	static bool InAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded(GetName());
	}

	static FName GetName()
	{
		return "EditorPlus";
	}

	TSharedPtr<FEditorPlusMenuManager> GetMenuManager() const
	{
		return MenuManager;
	}
	
private:
	TSharedPtr<FEditorPlusMenuManager> MenuManager;
};
