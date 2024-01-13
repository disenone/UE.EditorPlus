#pragma once

class IEditorPlusToolInterface: public TSharedFromThis<IEditorPlusToolInterface>
{
public:
	virtual ~IEditorPlusToolInterface() {}
	virtual void OnStartup() {}
	virtual void OnShutdown() {}
};

class IEditorPlusToolManagerInterface: public TSharedFromThis<IEditorPlusToolManagerInterface>
{
public:
	virtual ~IEditorPlusToolManagerInterface() {}
	
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
	}

	virtual void AddTools() = 0;
	
protected:
	TArray<TSharedRef<IEditorPlusToolInterface>> Tools;
};
