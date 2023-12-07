#include "EditorPlusPathMenuImpl.h"
#include "EditorPlusMenuManager.h"
#include "EditorPlusUtils.h"

FEditorPlusPathMenuBar::~FEditorPlusPathMenuBar()
{
	FEditorPlusPathMenuBar::Unregister();
	FEditorPlusPathMenuBar::RemoveMenuBarExtension();
}


void FEditorPlusPathMenuBar::Register(FMenuBarBuilder& MenuBarBuilder)
{
	MenuBarBuilder.AddPullDownMenu(
	FText::FromName(Name),
	FText(),
	FEditorPlusMenuManager::GetDelegate<FPathMenuExtensionDelegate>(Name));	
}

void FEditorPlusPathMenuBar::Unregister()
{
}

void FEditorPlusPathMenuBar::RegisterWithMerge(FMenuBuilder& MenuBuilder, const bool bMerge)
{
	for(const auto& Child: Children)
	{
		if(Child->HasMergeMenu(bMerge))
		{
			Child->RegisterWithMerge(MenuBuilder, bMerge);
		}
	}
}


void FEditorPlusPathMenuBar::DoAddMenuBarExtension(const FName& ExtensionHook, EExtensionHook::Position Position, const FName& UniId)
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
		FEditorPlusMenuManager::RegisterDelegate<FPathMenuBarExtensionDelegate>(UniqueId, FPathMenuBarExtensionDelegate::CreateSP(this, &FEditorPlusMenuBase::OnMenuBarExtension)));
	FEditorPlusUtils::GetLevelEditorModule().GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	
	FEditorPlusMenuManager::RegisterDelegate<FPathMenuExtensionDelegate>(UniqueId, FNewMenuDelegate::CreateSP(this, &FEditorPlusMenuBase::OnMenuExtension));
	
	ModuleChangedHandler = FModuleManager::Get().OnModulesChanged().AddSP(this, &FEditorPlusMenuBase::OnMenuBarModuleChanged);
}


void FEditorPlusPathMenuBar::OnMenuBarExtension(FMenuBarBuilder& MenuBarBuilder)
{
	if (FEditorPlusMenuManager::ContainsDelegate<FMenuBarExtensionDelegate>(UniqueId) && !HasMergeMenu(false)) return;
	
	FEditorPlusMenuBase::OnMenuBarExtension(MenuBarBuilder);
}

void FEditorPlusPathMenuBar::RemoveMenuBarExtension()
{
	if (MenuExtender.IsValid())
	{
		FEditorPlusUtils::GetLevelEditorModule().GetMenuExtensibilityManager()->RemoveExtender(MenuExtender);
		FEditorPlusMenuManager::RemoveDelegate<FPathMenuBarExtensionDelegate>(UniqueId);
		FEditorPlusMenuManager::RemoveDelegate<FPathMenuExtensionDelegate>(UniqueId);
		if (ModuleChangedHandler.IsValid())
		{
			FModuleManager::Get().OnModulesChanged().Remove(ModuleChangedHandler);
			ModuleChangedHandler.Reset();
		}
		MenuExtender.Reset();
	}	
}

void FEditorPlusPathMenuBar::OnMenuModuleChanged(FName ModuleThatChanged, EModuleChangeReason ReasonForChange)
{
	if (ModuleThatChanged == FEditorPlusModule::GetName() && ReasonForChange == EModuleChangeReason::ModuleLoaded)
	{
		FEditorPlusMenuManager::UpdateDelegate<FPathMenuExtensionDelegate>(
			UniqueId, FMenuExtensionDelegate::CreateSP(this, &FEditorPlusMenuBase::OnMenuExtension));
	}		
}

void FEditorPlusPathMenuBar::OnMenuExtension(FMenuBuilder& MenuBuilder)
{
	const bool CanMerge = FEditorPlusMenuManager::ContainsDelegate<FMenuBarExtensionDelegate>(UniqueId);

	if(CanMerge)
	{
		FEditorPlusMenuManager::ExecutePathMenu(GetName(), MenuBuilder, {}, false);
	}
	else
	{
		FEditorPlusMenuBase::OnMenuExtension(MenuBuilder);
	}
}


void FEditorPlusPathSubMenu::Register(FMenuBuilder& MenuBuilder, const FName& ParentPath)
{
	MenuBuilder.AddSubMenu(
		FText::FromName(Name),
		FText(),
		FNewMenuDelegate::CreateSP(this, &FEditorPlusPathSubMenu::MakeSubMenu)
	);
}

void FEditorPlusPathSubMenu::RegisterWithMerge(FMenuBuilder& MenuBuilder, const bool bMerge)
{
	MenuBuilder.AddSubMenu(
		FText::FromName(Name),
		FText(),
		FNewMenuDelegate::CreateSP(this, &FEditorPlusPathSubMenu::MakeSubMenu, bMerge)
	);	
}


void FEditorPlusPathSubMenu::MakeSubMenu(FMenuBuilder& MenuBuilder)
{
	FEditorPlusMenuBase::Register(MenuBuilder, NAME_None);
}


void FEditorPlusPathSubMenu::MakeSubMenu(FMenuBuilder& MenuBuilder, const bool bMerge)
{
	for(const auto& Child: Children)
	{
		if(Child->HasMergeMenu(bMerge))
		{
			Child->RegisterWithMerge(MenuBuilder, bMerge);
		}
	}	
}


FEditorPlusPathCommand::FEditorPlusPathCommand(const FName& Name, const FExecuteAction& ExecuteAction, const bool bMergeSubMenu)
	: CommandInfo(MakeShared<FEditorPlusCommandInfo>(Name, ExecuteAction)), bMergeSubMenu(bMergeSubMenu)
{
}

FEditorPlusPathCommand::~FEditorPlusPathCommand()
{
	FEditorPlusPathCommand::Unregister();
}

void FEditorPlusPathCommand::Register(FMenuBuilder& MenuBuilder, const FName& ParentPath)
{
	MenuBuilder.AddMenuEntry(
		FText::FromName(CommandInfo->Name), FText::FromName(CommandInfo->Desc), CommandInfo->Icon,
		CommandInfo->ExecuteAction, NAME_None, CommandInfo->Type);
}

void FEditorPlusPathCommand::Unregister()
{
}
