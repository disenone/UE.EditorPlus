// Copyright (c) 2023 - 2024, Disenone (https://github.com/disenone).
// All rights reserved. Licensed under MIT License.

#pragma once

class IEditorPlusToolManagerInterface: public TSharedFromThis<IEditorPlusToolManagerInterface>
{
public:
	virtual ~IEditorPlusToolManagerInterface() {}

	virtual void OnStartup() {StartupTools();}
	virtual void OnShutdown() {ShutdownTools();}

	virtual void StartupTools()
	{
		if (IsRunningCommandlet())
			return;
		AddTools();
		for (const auto& Tool: Tools)
		{
			Tool->OnStartup();
		}
	}

	virtual void ShutdownTools()
	{
		if (IsRunningCommandlet())
			return;
		for (const auto& Tool: Tools)
		{
			Tool->OnShutdown();
		}
		Tools.Empty();
	}

	virtual void AddTools() {};
	
protected:
	TArray<TSharedRef<IEditorPlusToolManagerInterface>> Tools;
};

using IEditorPlusToolInterface = IEditorPlusToolManagerInterface;
