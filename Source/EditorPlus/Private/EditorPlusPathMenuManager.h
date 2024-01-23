// Copyright (c) 2023 - 2024, Disenone (https://github.com/disenone).
// All rights reserved. Licensed under MIT License.

#pragma once
#include "EditorPlusMenu.h"

class FEditorPlusPathMenuManager: public TSharedFromThis<FEditorPlusPathMenuManager>
{
public:
	TSharedPtr<FEditorPlusMenuBase> RegisterPath(const FString& Path, const TSharedPtr<FEditorPlusMenuBase>& Menu=nullptr);
	TSharedPtr<FEditorPlusMenuBase> RegisterPath(const FString& Path, const FText& FriendlyName, const FText& FriendlyTips);
	TSharedPtr<FEditorPlusMenuBase> RegisterAction(
		const FString& Path, const FExecuteAction& ExecuteAction, const FName& Hook=EP_FNAME_HOOK_AUTO,
		const FText& FriendlyName=FText::GetEmpty(), const FText& FriendlyTips=FText::GetEmpty());

	TSharedPtr<FEditorPlusMenuBase> RegisterChildPath(
		const TSharedRef<FEditorPlusMenuBase>& InParent, const FString& Path, const TSharedPtr<FEditorPlusMenuBase>& Menu=nullptr);
	TSharedPtr<FEditorPlusMenuBase> RegisterChildPath(
		const TSharedRef<FEditorPlusMenuBase>& InParent, const FString& Path, const FText& FriendlyName, const FText& FriendlyTips);
	TSharedPtr<FEditorPlusMenuBase> RegisterChildAction(
		const TSharedRef<FEditorPlusMenuBase>& InParent, const FString& Path, const FExecuteAction& ExecuteAction,
		const FName& Hook=EP_FNAME_HOOK_AUTO, const FText& FriendlyName=FText::GetEmpty(), const FText& FriendlyTips=FText::GetEmpty());

	bool Unregister(const FString& Path, const TSharedPtr<FEditorPlusMenuBase>& Leaf=nullptr);
	bool Unregister(const FString& Path, const FName& UniqueId);
	
	TSharedPtr<FEditorPlusMenuBase> GetNodeByPath(const FString& Path);
protected:
	
	// change path into standard format array ["<Hook>hook", "<tag2>name2", ...]
	static TArray<FString> NormalizeSplitPath(const FString& Path, const bool AutoInsertHook=true, const bool AutoInsertBar=true);
	
	// change path into standard format "/<Hook>hook/<tag2>name2/..."
	static FString NormalizePath(const FString& Path);
	
	TSharedPtr<FEditorPlusMenuBase> DoRegister(
		const TArray<FString>& NormalizedPathNames, const TSharedPtr<FEditorPlusMenuBase>& Node=nullptr,
		const FText& FriendlyName=FText::GetEmpty(), const FText& FriendlyTips=FText::GetEmpty());
	
	TSharedPtr<FEditorPlusMenuBase> DoRegister(
		const FString& Path, const TSharedPtr<FEditorPlusMenuBase>& Node=nullptr,
		const FText& FriendlyName=FText::GetEmpty(), const FText& FriendlyTips=FText::GetEmpty())
	{
		return DoRegister(NormalizeSplitPath(Path), Node, FriendlyName, FriendlyTips);
	}

	TSharedPtr<FEditorPlusMenuBase> DoRegister(
		const TSharedRef<FEditorPlusMenuBase>& InParent, const TArray<FString>& NormalizedPathNames, const TSharedPtr<FEditorPlusMenuBase>& Node=nullptr,
		const FText& FriendlyName=FText::GetEmpty(), const FText& FriendlyTips=FText::GetEmpty());

	TSharedPtr<FEditorPlusMenuBase> DoRegister(
		const TSharedRef<FEditorPlusMenuBase>& InParent, const FString& Path, const TSharedPtr<FEditorPlusMenuBase>& Node=nullptr,
		const FText& FriendlyName=FText::GetEmpty(), const FText& FriendlyTips=FText::GetEmpty())
	{
		return DoRegister(InParent, NormalizeSplitPath(Path, false, false), Node, FriendlyName, FriendlyTips);
	}

	TSharedPtr<FEditorPlusHook> MakeRoot(const FString& PathName);	
	TSharedPtr<FEditorPlusMenuBase> MakeNode(
		const TSharedRef<FEditorPlusMenuBase>& Parent, const FString& PathName, bool IsLeaf,
		const TSharedPtr<FEditorPlusMenuBase>& Node=nullptr, const FText& FriendlyName=FText::GetEmpty(),
		const FText& FriendlyTips=FText::GetEmpty());

	bool RemoveNode(
		const TSharedRef<FEditorPlusMenuBase>& Parent, const TArray<FString>& Paths,
		const int ParentIdx, const TSharedPtr<FEditorPlusMenuBase>& Leaf=nullptr);
	
	bool RemoveNode(
		const TSharedRef<FEditorPlusMenuBase>& Parent, const TArray<FString>& Paths,
		const int ParentIdx, const FName& UniqueId);
protected:	
	TMap<FString, TSharedRef<FEditorPlusHook>> RootMap;
};
