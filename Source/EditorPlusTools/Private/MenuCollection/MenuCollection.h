#pragma once

#include "EditorPlusToolInterface.h"
#include "EditorPlusMenu.h"

class FMenuCollection: public IEditorPlusToolInterface
{
public:
	virtual void OnStartup() override;
	virtual void OnShutdown() override;
private:
	void BuildMenu();
	void BuildSubMenu();
	void BuildPathMenu();
	
};

DECLARE_LOG_CATEGORY_EXTERN(LogMenuCollection, Display, All);
