// Copyright (c) 2023 - 2024, Disenone (https://github.com/disenone).
// All rights reserved. Licensed under MIT License.

#pragma once

#include "EditorPlusToolInterface.h"
#include "EditorPlusMenu.h"
#include "EditorPlusTab.h"

class SClassBrowserTab;

class FClassBrowser final: public IEditorPlusToolInterface
{
public:
	virtual void OnStartup() override;
	virtual void OnShutdown() override;

private:
	void RegisterTab();
	void UnregisterTab();
	void RegisterMenu();
	void UnregisterMenu();
	
	TSharedPtr<FEditorPlusTab> Tab;
	TSharedPtr<FEditorPlusMenuBase> Menu;
	TSharedPtr<FEditorPlusMenuBase> TestMenu;
	TSharedPtr<SClassBrowserTab> ClassBrowserTab;
};

DECLARE_LOG_CATEGORY_EXTERN(LogClassBrowser, Display, All);
