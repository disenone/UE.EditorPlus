
#include "EditorPlusMenu.h"
#include "LevelEditor.h"
#include "EditorPlusCommand.h"
#include "EditorPlusMenuManager.h"
#include "EditorPlus.h"
#include "EditorPlusUtils.h"


FEditorPlusMenuBase::~FEditorPlusMenuBase()
{
	FEditorPlusMenuBase::Unregister();
	FEditorPlusMenuBase::RemoveMenuExtension();
}

TArray<TSharedRef<FEditorPlusMenuBase>> FEditorPlusMenuBase::GetChildrenByName(const FName& Name, const FName& TypeName)
{
	TArray<TSharedRef<FEditorPlusMenuBase>> Ret;
	for (auto Menu: Children)
	{
		if (Menu->GetName() == Name && Menu->IsType(TypeName))
		{
			Ret.Push(Menu);
		}
	}
	return Ret;
}

TArray<TSharedRef<FEditorPlusMenuBase>> FEditorPlusMenuBase::GetChildrenByType(const FName& TypeName)
{
	TArray<TSharedRef<FEditorPlusMenuBase>> Ret;
	for (auto Menu: Children)
	{
		if (Menu->IsType(TypeName))
		{
			Ret.Push(Menu);
		}
	}
	return Ret;	
}


void FEditorPlusMenuBase::Unregister()
{
	if (Children.Num())
	{
		for (auto Menu: Children)
		{
			Menu->Unregister();
		}
		Children.Empty();	
	}
}

void FEditorPlusMenuBase::Register(FMenuBuilder& MenuBuilder, const FName& ParentPath)
{
	for (auto Menu: Children)
	{
		Menu->Register(MenuBuilder, ParentPath);
	}
}


void FEditorPlusMenuBase::DoAddMenuExtension(const FName& ExtensionHook, EExtensionHook::Position Position, const FName& UniId)
{
	if (MenuExtender.IsValid())
	{
		RemoveMenuExtension();
	}
	SetUniqueId(UniId);
	MenuExtender = MakeShared<FExtender>();
	MenuExtender->AddMenuExtension(
		ExtensionHook, Position,
		nullptr,
		FEditorPlusMenuManager::RegisterDelegate(UniqueId, FMenuExtensionDelegate::CreateSP(this, &FEditorPlusMenuBase::OnMenuExtension)));
	FEditorPlusUtils::GetLevelEditorModule().GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	ModuleChangedHandler = FModuleManager::Get().OnModulesChanged().AddSP(this, &FEditorPlusMenuBase::OnMenuModuleChanged);
}

void FEditorPlusMenuBase::OnMenuExtension(FMenuBuilder& MenuBuilder)
{
	Register(MenuBuilder, NAME_None);
}

void FEditorPlusMenuBase::OnMenuModuleChanged(FName ModuleThatChanged, EModuleChangeReason ReasonForChange)
{
	if (ModuleThatChanged == FEditorPlusModule::GetName() && ReasonForChange == EModuleChangeReason::ModuleLoaded)
	{
		FEditorPlusMenuManager::UpdateDelegate(UniqueId, FMenuExtensionDelegate::CreateSP(this, &FEditorPlusMenuBase::OnMenuExtension));
	}		
}

void FEditorPlusMenuBase::RemoveMenuExtension()
{
	if(MenuExtender.IsValid())
	{
		FEditorPlusMenuManager::RemoveDelegate<FMenuExtensionDelegate>(UniqueId);
		FEditorPlusMenuManager::RemoveDelegate<FMenuBarExtensionDelegate>(UniqueId);
		FEditorPlusUtils::GetLevelEditorModule().GetMenuExtensibilityManager()->RemoveExtender(MenuExtender);
		MenuExtender.Reset();
	}	
}


void FEditorPlusMenuBase::DoAddMenuBarExtension(const FName& ExtensionHook, EExtensionHook::Position Position, const FName& UniId)
{
	if(MenuExtender.IsValid())
	{
		RemoveMenuBarExtension();
	}
	SetUniqueId(UniId);
	MenuExtender = MakeShared<FExtender>();
	MenuExtender->AddMenuBarExtension(
		ExtensionHook, Position,
		nullptr,
		FEditorPlusMenuManager::RegisterDelegate(UniqueId, FMenuBarExtensionDelegate::CreateSP(this, &FEditorPlusMenuBase::OnMenuBarExtension)));
	FEditorPlusUtils::GetLevelEditorModule().GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	
	FEditorPlusMenuManager::RegisterDelegate(UniqueId, FNewMenuDelegate::CreateSP(this, &FEditorPlusMenuBase::OnMenuExtension));
	
	ModuleChangedHandler = FModuleManager::Get().OnModulesChanged().AddSP(this, &FEditorPlusMenuBase::OnMenuBarModuleChanged);
}


void FEditorPlusMenuBase::OnMenuBarExtension(FMenuBarBuilder& MenuBarBuilder)
{
	Register(MenuBarBuilder);
}

void FEditorPlusMenuBase::OnMenuBarModuleChanged(FName ModuleThatChanged, EModuleChangeReason ReasonForChange)
{
	OnMenuModuleChanged(ModuleThatChanged, ReasonForChange);
}

void FEditorPlusMenuBase::RemoveMenuBarExtension()
{
	if (MenuExtender.IsValid())
	{
		FEditorPlusUtils::GetLevelEditorModule().GetMenuExtensibilityManager()->RemoveExtender(MenuExtender);
		FEditorPlusMenuManager::RemoveDelegate<FMenuExtensionDelegate>(UniqueId);
		FEditorPlusMenuManager::RemoveDelegate<FMenuBarExtensionDelegate>(UniqueId);
		if (ModuleChangedHandler.IsValid())
		{
			FModuleManager::Get().OnModulesChanged().Remove(ModuleChangedHandler);
			ModuleChangedHandler.Reset();
		}
		MenuExtender.Reset();
	}	
}

bool FEditorPlusMenuBase::HasMergeMenu(const bool bMerge) const
{
	for(const auto& Child: Children)
	{
		if(Child->HasMergeMenu(bMerge))
		{
			return true;
		}
	}
	return false;
}


void FEditorPlusSection::Register(FMenuBuilder& MenuBuilder, const FName& ParentPath)
{
	MenuBuilder.BeginSection(Hook, FText::FromName(Name));
	FEditorPlusMenuBase::Register(MenuBuilder, ParentPath);
	MenuBuilder.EndSection();
}


void FEditorPlusSeparator::Register(FMenuBuilder& MenuBuilder, const FName& ParentPath)
{
	MenuBuilder.AddMenuSeparator(Hook);
}


void FEditorPlusSubMenu::Register(FMenuBuilder& MenuBuilder, const FName& ParentPath)
{
	MenuBuilder.AddSubMenu(
		FText::FromName(Name),
		FText::FromName(Tips),
		FNewMenuDelegate::CreateSP(this, &FEditorPlusSubMenu::MakeSubMenu, ParentPath)
	);
}

void FEditorPlusSubMenu::MakeSubMenu(FMenuBuilder& MenuBuilder, const FName ParentPath)
{
	const FName Path = FEditorPlusUtils::PathJoin(ParentPath, GetName());	
	FEditorPlusMenuBase::Register(MenuBuilder, Path);
	
	TArray<FName> SubMenuNames;
	for(const auto& Child: GetChildrenByType<FEditorPlusSubMenu>())
	{
		SubMenuNames.Add(Child->GetName());
	}
	FEditorPlusMenuManager::ExecutePathMenu(Path, MenuBuilder, SubMenuNames, true);
}


FEditorPlusMenu::FEditorPlusMenu(
	TSharedRef<IEditorPlusCommandsInterface> Commands, const FName& UniqueName, const FName& FriendlyName,
	const FName& Desc, const FExecuteAction& ExecuteAction, const EUserInterfaceActionType& Type,
	const FInputChord& Chord, const FName& LoctextNamespace, const FSlateIcon& Icon)
{
	CommandMgr = Commands;
	CommandInfo = MakeShared<FEditorPlusCommandInfo>(
		UniqueName, ExecuteAction, FriendlyName, Desc, Type, Chord, LoctextNamespace, Icon);
}


FEditorPlusMenu::FEditorPlusMenu(
	const FName& Name, const FName& Desc, const FExecuteAction& ExecuteAction,
	const EUserInterfaceActionType& Type, const FSlateIcon& InIcon)
{
	CommandInfo = MakeShared<FEditorPlusCommandInfo>(
		Name, ExecuteAction, Name, Desc, Type,
		FInputChord(), NAME_None, InIcon);
}

FEditorPlusMenu::~FEditorPlusMenu()
{
	FEditorPlusMenu::Unregister();
}


void FEditorPlusMenu::Register(FMenuBuilder& MenuBuilder, const FName& ParentPath)
{
	if (CommandMgr.IsValid())
	{
		CommandMgr->AddCommand(CommandInfo.ToSharedRef());
		MenuBuilder.PushCommandList(CommandMgr->GetCommandList());
    	MenuBuilder.AddMenuEntry(CommandInfo->Info);
    	MenuBuilder.PopCommandList();	
	}
	else
	{
		MenuBuilder.AddMenuEntry(
			FText::FromName(CommandInfo->Name), FText::FromName(CommandInfo->Desc), CommandInfo->Icon,
			CommandInfo->ExecuteAction, NAME_None, CommandInfo->Type);
	}
}


void FEditorPlusMenu::Unregister()
{
	if (CommandMgr.IsValid() && CommandInfo.IsValid())
	{
		CommandMgr->RemoveCommand(CommandInfo.ToSharedRef());
	}
}


FEditorPlusMenuBar::~FEditorPlusMenuBar()
{
	FEditorPlusMenuBar::Unregister();
}


void FEditorPlusMenuBar::Register(FMenuBarBuilder& MenuBarBuilder)
{
	MenuBarBuilder.AddPullDownMenu(
	FText::FromName(Name),
	FText::FromName(Desc),
	FEditorPlusMenuManager::GetDelegate<FNewMenuDelegate>(UniqueId),
	Hook);
}

void FEditorPlusMenuBar::Unregister()
{
}

void FEditorPlusMenuBar::OnMenuExtension(FMenuBuilder& MenuBuilder)
{
	FEditorPlusMenuBase::Register(MenuBuilder, GetName());
	
	// Other PathMenu
	if (FEditorPlusMenuManager::ContainsDelegate<FPathMenuExtensionDelegate>(UniqueId))
	{
		TArray<FName> SubMenuNames;
		for(const auto& Child: GetChildrenByType<FEditorPlusSubMenu>())
		{
			SubMenuNames.Add(Child->GetName());
		}

		FEditorPlusMenuManager::ExecutePathMenu(GetName(), MenuBuilder, SubMenuNames, true);
		// FEditorPlusMenuManager::GetDelegate<FPathMenuExtensionDelegate>(UniqueId).ExecuteIfBound(MenuBuilder);
	}
}


void FEditorPlusWidget::Register(FMenuBuilder& MenuBuilder, const FName& ParentPath)
{
	MenuBuilder.AddWidget(Widget, FText::FromString(""));
}


void FEditorPlusToolMenu::Register(FMenuBuilder& MenuBuilder, const FName& ParentPath)
{
	Tool->OnBuildMenu(MenuBuilder);
}


void FEditorPlusToolMenu::Unregister()
{
	Tool->OnDestroyMenu();
}


FEditorPlusPathMenu::FEditorPlusPathMenu(const FName& Path, const FExecuteAction& ExecuteAction, const bool bMergeMenu)
	: Path(Path), ExecuteAction(ExecuteAction)
{
	Menu = FEditorPlusMenuManager::RegisterPath(Path, ExecuteAction, bMergeMenu);
}

FEditorPlusPathMenu::~FEditorPlusPathMenu()
{
	FEditorPlusMenuManager::UnregisterPath(Path, Menu.ToSharedRef());
}
