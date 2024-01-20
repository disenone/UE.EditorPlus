
#pragma once

#include "CoreMinimal.h"
#include "EditorPlusMenu.h"
#include "EditorPlusToolInterface.h"
#include "EditorPlusTools.h"

class FEditorPlusToolsImpl: public IEditorPlusToolManagerInterface
{
public:
	virtual void StartupTools() override;
	virtual void ShutdownTools() override;

	virtual void AddTools() override;

	static TSharedRef<FEditorPlusToolsImpl> Get()
	{
		if (!Instance.IsValid())
		{
			TSharedRef<FEditorPlusToolsImpl> Impl = MakeShared<FEditorPlusToolsImpl>();
			Instance = Impl;
			return Impl;
		}
		return Instance.Pin().ToSharedRef();
	}

	static bool InAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded(GetName());
	}

	static FName GetName()
	{
		return FEditorPlusToolsModule::GetName();
	}

protected:
	FEditorPlusToolsImpl() {}
	FEditorPlusToolsImpl(const FEditorPlusToolsImpl&) = delete;
	FEditorPlusToolsImpl& operator=(const FEditorPlusToolsImpl&) = delete;
	
	void RegisterConsoleCommand();
	
private:
	TSharedPtr<FEditorPlusMenuBase> Root;

	static TWeakPtr<FEditorPlusToolsImpl> Instance;
	
	template <typename ObjectType, ESPMode Mode> friend class SharedPointerInternals::TIntrusiveReferenceController;
};
