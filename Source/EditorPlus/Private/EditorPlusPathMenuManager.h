#pragma once
#include "EditorPlusMenu.h"

class FEditorPlusPathMenuManager: public TSharedFromThis<FEditorPlusPathMenuManager>
{
public:
	TSharedPtr<FEditorPlusMenuBase> RegisterPath(const FString& Path, const TSharedPtr<FEditorPlusMenuBase>& Menu=nullptr);
	TSharedPtr<FEditorPlusMenuBase> RegisterAction(const FString& Path, const FExecuteAction& ExecuteAction, const FName& Hook="");
	
	bool Unregister(const FString& Path, const TSharedPtr<FEditorPlusMenuBase>& Leaf=nullptr);
	bool Unregister(const FString& Path, const FName& UniqueId);
	
	TSharedPtr<FEditorPlusMenuBase> GetNodeByPath(const FString& Path);
protected:
	
	// change path into standard format array ["<Hook>hook", "<tag2>name2", ...]
	static TArray<FString> NormalizeSplitPath(const FString& Path);
	
	// change path into standard format "/<Hook>hook/<tag2>name2/..."
	static FString NormalizePath(const FString& Path);
	
	TSharedPtr<FEditorPlusMenuBase> DoRegister(
		const TArray<FString>& NormalizedPathNames, const TSharedPtr<FEditorPlusMenuBase>& Node=nullptr);
	
	TSharedPtr<FEditorPlusMenuBase> DoRegister(const FString& Path, const TSharedPtr<FEditorPlusMenuBase>& Node=nullptr)
	{
		return DoRegister(NormalizeSplitPath(Path), Node);
	}

	TSharedPtr<FEditorPlusHook> MakeRoot(const FString& PathName);	
	TSharedPtr<FEditorPlusMenuBase> MakeNode(
		const TSharedRef<FEditorPlusMenuBase>& Parent, const FString& PathName, const TSharedPtr<FEditorPlusMenuBase>& Node=nullptr);

	bool RemoveNode(
		const TSharedRef<FEditorPlusMenuBase>& Parent, const TArray<FString>& Paths,
		const int ParentIdx, const TSharedPtr<FEditorPlusMenuBase>& Leaf=nullptr);
	
	bool RemoveNode(
		const TSharedRef<FEditorPlusMenuBase>& Parent, const TArray<FString>& Paths,
		const int ParentIdx, const FName& UniqueId);
protected:	
	TMap<FString, TSharedRef<FEditorPlusHook>> RootMap;
};
