#include "EditorPlusPathMenuManager.h"
#include "EditorPlusUtils.h"


TArray<FString> CheckAndSplitPath(const FName& Path, const int32 MinLen = 0)
{
	checkf(!Path.IsNone(), TEXT("Path is None"));

	const FString SPath = Path.ToString();

	TArray<FString> Split = FEditorPlusUtils::SplitString(SPath, "/");

	checkf(Split.Num() >= MinLen, TEXT("Path must have at least %d Length, ag: /a/b"), MinLen);

	return Split;
}


TSharedPtr<FEditorPlusMenuBase> FEditorPlusPathMenuManager::Register(const FName& Path, const FExecuteAction& ExecuteAction, bool bMergeSubMenu)
{
	TArray<FString>	Split = CheckAndSplitPath(Path, 2);

	const FName BarName(Split[0]);

	// Build MenuBar
	TSharedPtr<FEditorPlusPathMenuBar> Bar;
	if (BarMap.Contains(BarName))
	{
		Bar = BarMap[BarName];
	}
	else
	{
		Bar = MakeShared<FEditorPlusPathMenuBar>(BarName);
		BarMap.Add(BarName, Bar.ToSharedRef());
		Bar->AddMenuBarExtension("Help", EExtensionHook::After);
	}

	// Build SubMenu
	TSharedPtr<FEditorPlusMenuBase> Parent = Bar;
	for (int i = 1; i < Split.Num() - 1; ++i)
	{
		const FName SubMenuName(Split[i]);
		TArray<TSharedRef<FEditorPlusPathSubMenu>> Subs = Parent->GetChildrenByName<FEditorPlusPathSubMenu>(SubMenuName);

		if (!Subs.Num())
		{
			TSharedPtr<FEditorPlusMenuBase> SubMenu = MakeShared<FEditorPlusPathSubMenu>(SubMenuName);
			Parent->AddChild(SubMenu.ToSharedRef());
			Parent = SubMenu;
		}
		else
		{
			Parent = Subs[0];
		}
	}

	// Build Command Menu
	TSharedRef<FEditorPlusPathCommand> Command = MakeShared<FEditorPlusPathCommand>(
		FName(Split[Split.Num() - 1]), ExecuteAction, bMergeSubMenu);
	Parent->AddChild(Command);
	return Command;
}


bool RemoveCommand(TSharedRef<FEditorPlusMenuBase> Parent, const TArray<FString>& Paths, const int ParentIdx, const TSharedRef<FEditorPlusMenuBase>& Command)
{
	// DFS Remove Command
	if (ParentIdx >= Paths.Num() - 1) return false;

	const FName ChildName(Paths[ParentIdx + 1]);

	if(ParentIdx == Paths.Num() - 2)
	{
		if(Parent->RemoveChild(Command))
		{
			return true;
		}
		return false;
	}
	
	for(const auto& Child: Parent->GetChildrenByName(ChildName))
	{
		if(RemoveCommand(Child, Paths, ParentIdx + 1, Command))
		{
			if(Child->IsEmpty())
			{
				Parent->RemoveChild(Child);
			}
			return true;
		}
	}

	return false;
}


bool FEditorPlusPathMenuManager::Unregister(const FName& Path, const TSharedRef<FEditorPlusMenuBase>& Command)
{
	TArray<FString>	Split = CheckAndSplitPath(Path, 2);

	const FName BarName(Split[0]);
	
	if (!BarMap.Contains(BarName)) return false;

	if(RemoveCommand(BarMap[BarName], Split, 0, Command))
	{
		if(BarMap[BarName]->IsEmpty())
		{
			BarMap.Remove(BarName);
		}
		return true;
	}

	return false;
}


TSharedPtr<FEditorPlusMenuBase> FEditorPlusPathMenuManager::GetMenuByPath(const FName& Path)
{
	const TArray<FString> Split = CheckAndSplitPath(Path);

	if(!Split.Num()) return nullptr;

	if(!BarMap.Contains(FName(Split[0]))) return nullptr;
	
	TSharedRef<FEditorPlusMenuBase> CurrentMenu = BarMap[FName(Split[0])];

	for (int i = 1; i < Split.Num(); ++i)
	{
		const FName SubMenuName(Split[i]);
		TArray<TSharedRef<FEditorPlusPathSubMenu>> Subs = CurrentMenu->GetChildrenByName<FEditorPlusPathSubMenu>(SubMenuName);

		if(!Subs.Num()) return nullptr;

		CurrentMenu = Subs[0];
	}

	return CurrentMenu;
}



void FEditorPlusPathMenuManager::ExecutePathMenu(const FName& Path, FMenuBuilder& MenuBuilder, const TArray<FName>& MergedSubMenuNames, const bool bMerge)
{
	const TSharedPtr<FEditorPlusMenuBase> PathMenu = GetMenuByPath(Path);
	if(!PathMenu.IsValid()) return;

	if(!PathMenu->HasMergeMenu(bMerge)) return;

	for(const auto& Child: PathMenu->GetChildrenByType())
	{
		if(Child->IsType<FEditorPlusPathCommand>()
			|| (!MergedSubMenuNames.Contains(Child->GetName())
				&& PathMenu->HasMergeMenu(bMerge)))
		{
			Child->RegisterWithMerge(MenuBuilder, bMerge);
		}
	}
}
