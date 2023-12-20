#include "EditorPlusPath.h"
#include "EditorPlusMenuManager.h"

TSharedPtr<FEditorPlusMenuBase> FEditorPlusPath::RegisterPath(
	const FString& Path, const TSharedPtr<FEditorPlusMenuBase>& Menu)
{
	return FEditorPlusMenuManager::RegisterPath(Path, Menu);
}

TSharedPtr<FEditorPlusMenuBase> FEditorPlusPath::RegisterPathAction(
	const FString& Path, const FExecuteAction& ExecuteAction, const FName& Hook)
{
	return FEditorPlusMenuManager::RegisterPathAction(Path, ExecuteAction, Hook);
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