#pragma once
#include "EditorPlusPathMenuImpl.h"

class FEditorPlusPathMenuManager: public TSharedFromThis<FEditorPlusPathMenuManager>
{
public:
	TSharedPtr<FEditorPlusMenuBase> Register(const FName& Path, const FExecuteAction& ExecuteAction, bool bMergeSubMenu=false);
	bool Unregister(const FName& Path, const TSharedRef<FEditorPlusMenuBase>& Command);
	void ExecutePathMenu(const FName& Path, FMenuBuilder& MenuBuilder, const TArray<FName>& MergedSubMenuNames, const bool bMerge);

protected:
	TSharedPtr<FEditorPlusMenuBase> GetMenuByPath(const FName& Path);
	
	TMap<FName, TSharedRef<FEditorPlusPathMenuBar>> BarMap;
};
