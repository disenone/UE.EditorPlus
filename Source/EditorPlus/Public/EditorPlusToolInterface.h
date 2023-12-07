#pragma once

class IEditorPlusToolInterface: public TSharedFromThis<IEditorPlusToolInterface>
{
public:
	virtual ~IEditorPlusToolInterface() {}
	virtual void OnStartup() {};
	virtual void OnShutdown() {};
	virtual void OnBuildMenu(FMenuBuilder& MenuBuilder) {}
	virtual void OnDestroyMenu() {}
};

class IEditorPlusToolManagerInterface: public TSharedFromThis<IEditorPlusToolManagerInterface>
{
public:
	virtual ~IEditorPlusToolManagerInterface() {}
	
	virtual void StartupTools()
	{
		AddTools();
		for (const auto& Tool: Tools)
		{
			Tool->OnStartup();
		}
	}

	virtual void ShutdownTools()
	{
		for (const auto& Tool: Tools)
		{
			Tool->OnShutdown();
		}
	}

	virtual void AddTools() = 0;
	
protected:
	TArray<TSharedRef<IEditorPlusToolInterface>> Tools;
};
