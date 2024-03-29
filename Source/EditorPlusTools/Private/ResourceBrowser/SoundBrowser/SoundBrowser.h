﻿
#pragma once


#include "EditorPlusToolInterface.h"
#include "EditorPlusMenu.h"
#include "EditorPlusTab.h"

class SSoundBrowserTab;

class FSoundBrowser final: public IEditorPlusToolInterface
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
	TSharedPtr<SSoundBrowserTab> SoundBrowserTab;
};

DECLARE_LOG_CATEGORY_EXTERN(LogSoundBrowser, Display, All);
