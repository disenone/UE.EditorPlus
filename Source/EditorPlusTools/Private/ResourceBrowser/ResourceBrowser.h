
#pragma once

#include "EditorPlusToolInterface.h"
#include "EditorPlusMenu.h"

class FResourceBrowser final: public IEditorPlusToolManagerInterface
{
public:
	virtual void OnStartup() override;
	virtual void OnShutdown() override;
	virtual void AddTools() override;

private:
	void RegisterMenu();
	void UnregisterMenu();

	TSharedPtr<FEditorPlusMenuBase> Menu;
};
