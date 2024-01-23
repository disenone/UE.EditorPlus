// Copyright (c) 2023 - 2024, Disenone (https://github.com/disenone).
// All rights reserved. Licensed under MIT License.

#include "EditorPlusPath.h"
#include "EditorPlusMenuManager.h"

TSharedPtr<FEditorPlusMenuBase> FEditorPlusPath::RegisterPath(
	const FString& Path, const TSharedPtr<FEditorPlusMenuBase>& Menu)
{
	return FEditorPlusMenuManager::RegisterPath(Path, Menu);
}

TSharedPtr<FEditorPlusMenuBase> FEditorPlusPath::RegisterPath(const FString& Path, const FText& FriendlyName, const FText& FriendlyTips)
{
	return FEditorPlusMenuManager::RegisterPath(Path, FriendlyName, FriendlyTips);
}


TSharedPtr<FEditorPlusMenuBase> FEditorPlusPath::RegisterPathAction(
	const FString& Path, const FExecuteAction& ExecuteAction, const FName& Hook, const FText& FriendlyName, const FText& FriendlyTips)
{
	return FEditorPlusMenuManager::RegisterPathAction(Path, ExecuteAction, Hook, FriendlyName, FriendlyTips);
}

TSharedPtr<FEditorPlusMenuBase> FEditorPlusPath::RegisterChildPath(
	const TSharedRef<FEditorPlusMenuBase>& InParent, const FString& Path, const TSharedPtr<FEditorPlusMenuBase>& Menu)
{
	return FEditorPlusMenuManager::RegisterChildPath(InParent, Path, Menu);
}

TSharedPtr<FEditorPlusMenuBase>  FEditorPlusPath::RegisterChildPath(
	const TSharedRef<FEditorPlusMenuBase>& InParent, const FString& Path, const FText& FriendlyName, const FText& FriendlyTips)
{
	return FEditorPlusMenuManager::RegisterChildPath(InParent, Path, FriendlyName, FriendlyTips);
}

TSharedPtr<FEditorPlusMenuBase> FEditorPlusPath::RegisterChildPathAction(
	const TSharedRef<FEditorPlusMenuBase>& InParent, const FString& Path, const FExecuteAction& ExecuteAction,
	const FName& Hook, const FText& FriendlyName, const FText& FriendlyTips)
{
	return FEditorPlusMenuManager::RegisterChildPathAction(InParent, Path, ExecuteAction, Hook, FriendlyName, FriendlyTips);
}

bool FEditorPlusPath::UnregisterPath(
	const FString& Path, const TSharedPtr<FEditorPlusMenuBase>& Leaf)
{
	return FEditorPlusMenuManager::UnregisterPath(Path, Leaf);
}

TSharedPtr<FEditorPlusMenuBase> FEditorPlusPath::GetNodeByPath(const FString& Path)
{
	return FEditorPlusMenuManager::GetNodeByPath(Path);
}
