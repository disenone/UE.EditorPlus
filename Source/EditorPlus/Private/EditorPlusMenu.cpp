
#include "EditorPlusMenu.h"
#include "LevelEditor.h"
#include "EditorPlusMenuManager.h"
#include "EditorPlus.h"
#include "EditorPlusUtils.h"

// ---------------------------------------------------------------------
// region FEditorPlusMenuBase
//

FEditorPlusMenuBase::~FEditorPlusMenuBase()
{
	FEditorPlusMenuBase::Destroy();
}


void FEditorPlusMenuBase::Destroy(const bool UnregisterPath)
{
	if(IsDestroyed()) return;
	FEditorPlusMenuBase::Unregister();
	if (Children.Num())
	{
		for (const auto Menu: Children)
		{
			Menu->Destroy(UnregisterPath);
		}
	}
	Children.Empty();
	if (UnregisterPath && !Path.IsEmpty())
	{
		FEditorPlusMenuManager::UnregisterPath(Path, GetUniqueId());
	}
}


void FEditorPlusMenuBase::Register(FMenuBuilder& MenuBuilder)
{
	for (const auto Menu: Children)
	{
		Menu->Register(MenuBuilder);
	}
}


void FEditorPlusMenuBase::Unregister()
{
	if (Children.Num())
	{
		for (auto Menu: Children)
		{
			Menu->Unregister();
		}
	}
	FEditorPlusMenuBase::RemoveMenuExtension();
}


void FEditorPlusMenuBase::DoAddMenuExtension(const FName& ExtensionHook, const EExtensionHook::Position Position)
{
	if (MenuExtender.IsValid())
	{
		RemoveMenuExtension();
	}
	this->ExtHook = ExtensionHook;
	
	MenuExtender = MakeShared<FExtender>();
	MenuExtender->AddMenuExtension(
		ExtensionHook, Position,
		nullptr,
		FEditorPlusMenuManager::RegisterDelegate(
			GetUniqueId(),
			FMenuExtensionDelegate::CreateSP(this, &FEditorPlusMenuBase::OnMenuExtension))
	);
	
	FEditorPlusUtils::GetLevelEditorModule().GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	ModuleChangedHandler = FModuleManager::Get().OnModulesChanged().AddSP(this, &FEditorPlusMenuBase::OnMenuModuleChanged);

	FEditorPlusMenuManager::RegisterPath(Path, AsShared());
}

void FEditorPlusMenuBase::OnMenuExtension(FMenuBuilder& MenuBuilder)
{
	Register(MenuBuilder);
}

void FEditorPlusMenuBase::OnMenuModuleChanged(FName ModuleThatChanged, EModuleChangeReason ReasonForChange)
{
	if (ModuleThatChanged == FEditorPlusModule::GetName() && ReasonForChange == EModuleChangeReason::ModuleLoaded)
	{
		FEditorPlusMenuManager::UpdateDelegate(GetUniqueId(), FMenuExtensionDelegate::CreateSP(this, &FEditorPlusMenuBase::OnMenuExtension));
	}		
}

void FEditorPlusMenuBase::RemoveMenuExtension()
{
	if(MenuExtender.IsValid())
	{
		FEditorPlusMenuManager::RemoveDelegate<FMenuExtensionDelegate>(GetUniqueId());
		FEditorPlusMenuManager::RemoveDelegate<FMenuBarExtensionDelegate>(GetUniqueId());
		FEditorPlusMenuManager::UnregisterPath(ExtHook.ToString(), GetUniqueId());
		FEditorPlusUtils::GetLevelEditorModule().GetMenuExtensibilityManager()->RemoveExtender(MenuExtender);
		MenuExtender.Reset();
		if (ModuleChangedHandler.IsValid())
		{
			FModuleManager::Get().OnModulesChanged().Remove(ModuleChangedHandler);
			ModuleChangedHandler.Reset();
		}
	}	
}


void FEditorPlusMenuBase::DoAddMenuBarExtension(const FName& ExtensionHook, const EExtensionHook::Position Position)
{
	if(MenuExtender.IsValid())
	{
		RemoveMenuBarExtension();
	}
	MenuExtender = MakeShared<FExtender>();
	MenuExtender->AddMenuBarExtension(
		ExtensionHook, Position,
		nullptr,
		FEditorPlusMenuManager::RegisterDelegate(
			GetUniqueId(),
			FMenuBarExtensionDelegate::CreateSP(this, &FEditorPlusMenuBase::OnMenuBarExtension))
	);
	
	FEditorPlusUtils::GetLevelEditorModule().GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	
	FEditorPlusMenuManager::RegisterDelegate(GetUniqueId(), FNewMenuDelegate::CreateSP(this, &FEditorPlusMenuBase::OnMenuExtension));

	ModuleChangedHandler = FModuleManager::Get().OnModulesChanged().AddSP(this, &FEditorPlusMenuBase::OnMenuBarModuleChanged);
	
	FEditorPlusMenuManager::RegisterPath(Path, AsShared());
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
		FEditorPlusMenuManager::RemoveDelegate<FMenuExtensionDelegate>(GetUniqueId());
		FEditorPlusMenuManager::RemoveDelegate<FMenuBarExtensionDelegate>(GetUniqueId());
		FEditorPlusMenuManager::UnregisterPath(ExtHook.ToString(), GetUniqueId());
		MenuExtender.Reset();
		if (ModuleChangedHandler.IsValid())
		{
			FModuleManager::Get().OnModulesChanged().Remove(ModuleChangedHandler);
			ModuleChangedHandler.Reset();
		}
	}	
}

TSharedPtr<FEditorPlusMenuBase> FEditorPlusMenuBase::CreateByPathName(const FString& PathName)
{
	auto TypeAndName = FEditorPlusMenuBase::GetTypeAndNameByPathName(PathName);
	const FName Name = FName(TypeAndName.Get<1>());
	
	switch(TypeAndName.Get<0>())
	{
		case EEditorPlusMenuType::Hook:
			return MakeShared<FEditorPlusHook>(Name);
		case EEditorPlusMenuType::MenuBar:
			return MakeShared<FEditorPlusMenuBar>(Name, Name, Name);
		case EEditorPlusMenuType::SubMenu:
			return MakeShared<FEditorPlusSubMenu>(Name, Name, Name);
		case EEditorPlusMenuType::Section:
			return MakeShared<FEditorPlusSection>(Name, Name, Name);
		case EEditorPlusMenuType::Separator:
			return MakeShared<FEditorPlusSeparator>(Name);
		case EEditorPlusMenuType::Command:
			return MakeShared<FEditorPlusCommand>(Name, Name, Name);
		default:
			return nullptr;
	}
}

//
// endregion FEditorPlusMenuBase
// ---------------------------------------------------------------------


// ---------------------------------------------------------------------
// region FEditorPlusMenuBar
//

FEditorPlusMenuBar::~FEditorPlusMenuBar()
{
	FEditorPlusMenuBar::Unregister();
}


void FEditorPlusMenuBar::Register(FMenuBarBuilder& MenuBarBuilder)
{
	MenuBarBuilder.AddPullDownMenu(
	FText::FromName(Name),
	FText::FromName(Tips),
	FEditorPlusMenuManager::GetDelegate<FNewMenuDelegate>(GetUniqueId()),
	Hook);
}


void FEditorPlusMenuBar::OnMenuExtension(FMenuBuilder& MenuBuilder)
{
	FEditorPlusMenuBase::Register(MenuBuilder);
}

//
// endregion FEditorPlusMenuBar
// ---------------------------------------------------------------------



// ---------------------------------------------------------------------
// region FEditorPlusMenuSection
//

void FEditorPlusSection::Register(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.BeginSection(Hook, FText::FromName(Name));
	FEditorPlusMenuBase::Register(MenuBuilder);
	MenuBuilder.EndSection();
}

//
// endregion FEditorPlusMenuSection
// ---------------------------------------------------------------------

// ---------------------------------------------------------------------
// region FEditorPlusSeparator
//

void FEditorPlusSeparator::Register(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuSeparator(Hook);
	FEditorPlusMenuBase::Register(MenuBuilder);
}

//
// endregion FEditorPlusSeparator
// ---------------------------------------------------------------------

// ---------------------------------------------------------------------
// region FEditorPlusSubMenu
//

void FEditorPlusSubMenu::Register(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddSubMenu(
		FText::FromName(Name),
		FText::FromName(Tips),
		FNewMenuDelegate::CreateSP(this, &FEditorPlusSubMenu::MakeSubMenu),
		false,
		FSlateIcon(),
		true,
		Hook
	);
}

void FEditorPlusSubMenu::MakeSubMenu(FMenuBuilder& MenuBuilder)
{
	FEditorPlusMenuBase::Register(MenuBuilder);
}

//
// endregion FEditorPlusSubMenu
// ---------------------------------------------------------------------

// ---------------------------------------------------------------------
// region FEditorPlusCommand
//

TSharedRef<FEditorPlusMenuBase> FEditorPlusCommand::BindAction(
	const TSharedRef<IEditorPlusCommandsInterface>& _CommandMgr,
	const FExecuteAction& ExecuteAction,
	const FName& FriendlyName,
	const EUserInterfaceActionType& Type,
	const FInputChord& Chord,
	const FName& LoctextNamespace,
	const FSlateIcon& Icon)
{
	this->CommandMgr = _CommandMgr;
	BindAction(ExecuteAction, FriendlyName, Type, Chord, LoctextNamespace, Icon);
	return AsShared();
}

TSharedRef<FEditorPlusMenuBase> FEditorPlusCommand::BindAction(
	const FExecuteAction& ExecuteAction,
	const FName& FriendlyName,
	const EUserInterfaceActionType& Type,
	const FInputChord& Chord,
	const FName& LoctextNamespace,
	const FSlateIcon& Icon)
{
	CommandInfo = MakeShared<FEditorPlusCommandInfo>(
		Name, ExecuteAction, FriendlyName, Tips, Hook, Type,
		Chord, LoctextNamespace, Icon);
	return AsShared();
}


FEditorPlusCommand::~FEditorPlusCommand()
{
	FEditorPlusCommand::Unregister();
}


void FEditorPlusCommand::Register(FMenuBuilder& MenuBuilder)
{
	if (!CommandInfo.IsValid()) return;
	
	if (CommandMgr.IsValid())
	{
		CommandMgr->AddCommand(CommandInfo.ToSharedRef());
		MenuBuilder.PushCommandList(CommandMgr->GetCommandList());
    	MenuBuilder.AddMenuEntry(CommandInfo->Info, CommandInfo->Hook);
    	MenuBuilder.PopCommandList();	
	}
	else
	{
		MenuBuilder.AddMenuEntry(
			FText::FromName(CommandInfo->Name), FText::FromName(CommandInfo->Tips), CommandInfo->Icon,
			CommandInfo->ExecuteAction, CommandInfo->Hook, CommandInfo->Type);
	}
}


void FEditorPlusCommand::Unregister()
{
	if (CommandMgr.IsValid() && CommandInfo.IsValid())
	{
		CommandMgr->RemoveCommand(CommandInfo.ToSharedRef());
	}
}
//
// endregion FEditorPlusCommand
// ---------------------------------------------------------------------
