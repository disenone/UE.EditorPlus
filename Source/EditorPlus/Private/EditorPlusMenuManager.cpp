#include "EditorPlusMenuManager.h"
#include "EditorPlusPathMenuManager.h"

FEditorPlusMenuManager::FEditorPlusMenuManager()
{
	PathMenuManager = MakeShared<FEditorPlusPathMenuManager>();
}

TSharedPtr<FEditorPlusMenuBase> FEditorPlusMenuManager::RegisterPath(
	const FString& Path, const TSharedPtr<FEditorPlusMenuBase>& Menu)
{
	if(!Get().IsValid()) return nullptr;
	return Get()->PathMenuManager->RegisterPath(Path, Menu);
}

TSharedPtr<FEditorPlusMenuBase> FEditorPlusMenuManager::RegisterPath(const FString& Path, const FText& FriendlyName, const FText& FriendlyTips)
{
	if(!Get().IsValid()) return nullptr;
	return Get()->PathMenuManager->RegisterPath(Path, FriendlyName, FriendlyTips);
}


TSharedPtr<FEditorPlusMenuBase> FEditorPlusMenuManager::RegisterPathAction(
	const FString& Path, const FExecuteAction& ExecuteAction, const FName& Hook, const FText& FriendlyName, const FText& FriendlyTips)
{
	if(!Get().IsValid()) return nullptr;
	return Get()->PathMenuManager->RegisterAction(Path, ExecuteAction, Hook, FriendlyName, FriendlyTips);
}

TSharedPtr<FEditorPlusMenuBase> FEditorPlusMenuManager::RegisterChildPath(
	const TSharedRef<FEditorPlusMenuBase>& InParent, const FString& Path, const TSharedPtr<FEditorPlusMenuBase>& Menu)
{
	if(!Get().IsValid()) return nullptr;
	return Get()->PathMenuManager->RegisterChildPath(InParent, Path, Menu);
}

TSharedPtr<FEditorPlusMenuBase> FEditorPlusMenuManager::RegisterChildPath(
	const TSharedRef<FEditorPlusMenuBase>& InParent, const FString& Path, const FText& FriendlyName, const FText& FriendlyTips)
{
	if(!Get().IsValid()) return nullptr;
	return Get()->PathMenuManager->RegisterChildPath(InParent, Path, FriendlyName, FriendlyTips);
}

TSharedPtr<FEditorPlusMenuBase> FEditorPlusMenuManager::RegisterChildPathAction(
	const TSharedRef<FEditorPlusMenuBase>& InParent, const FString& Path, const FExecuteAction& ExecuteAction,
	const FName& Hook, const FText& FriendlyName, const FText& FriendlyTips)
{
	if(!Get().IsValid()) return nullptr;
	return Get()->PathMenuManager->RegisterChildAction(InParent, Path, ExecuteAction, Hook, FriendlyName, FriendlyTips);
}

bool FEditorPlusMenuManager::UnregisterPath(const FString& Path, const TSharedPtr<FEditorPlusMenuBase>& Leaf)
{
	if(!Get().IsValid()) return false;
	return Get()->PathMenuManager->Unregister(Path, Leaf);
}

bool FEditorPlusMenuManager::UnregisterPath(const FString& Path, const FName& UniqueId)
{
	if(!Get().IsValid()) return false;
	return Get()->PathMenuManager->Unregister(Path, UniqueId);	
}

TSharedPtr<FEditorPlusMenuBase> FEditorPlusMenuManager::GetNodeByPath(const FString& Path)
{
	if(!Get().IsValid()) return nullptr;
	return Get()->PathMenuManager->GetNodeByPath(Path);
}
