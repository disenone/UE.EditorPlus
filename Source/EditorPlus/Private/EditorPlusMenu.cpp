﻿// Copyright (c) 2023 - 2024, Disenone (https://github.com/disenone).
// All rights reserved. Licensed under MIT License.

#include "EditorPlusMenu.h"
#include "LevelEditor.h"
#include "EditorPlusMenuManager.h"
#include "EditorPlus.h"
#include "EditorPlusPath.h"
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
	FEditorPlusMenuBase::UnregisterExtension();
	if (Children.Num())
	{
		// need to copy, because Children will be deleted in loop
		for (const auto Menu: TArray(Children))
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

void FEditorPlusMenuBase::PreBuildMenu(FMenuBuilder& MenuBuilder)
{
	for (auto& Subscriber: PreBuildMenuSubscribers)
	{
		Subscriber.Get<1>().ExecuteIfBound(MenuBuilder);
	}
}


void FEditorPlusMenuBase::BuildMenu(FMenuBuilder& MenuBuilder)
{
	PreBuildMenu(MenuBuilder);
	for (const auto Menu: Children)
	{
		Menu->BuildMenu(MenuBuilder);
	}
}

TSharedRef<FEditorPlusMenuBase> FEditorPlusMenuBase::RegisterPath()
{
	
	FEditorPlusMenuManager::RegisterPath(GetPath(), AsShared());
	return AsShared();
}


void FEditorPlusMenuBase::RegisterMenuExtension(const FName& ExtensionHook, const EExtensionHook::Position Position)
{
	UnregisterExtension();
	this->ExtHook = ExtensionHook;
	
	MenuExtender = MakeShared<FExtender>();
	MenuExtender->AddMenuExtension(
		ExtensionHook, Position,
		nullptr,
		FEditorPlusMenuManager::RegisterDelegate<FMenuExtensionDelegate>(
			GetUniqueId(),
			FMenuExtensionDelegate::CreateSP(this, &FEditorPlusMenuBase::OnMenuExtension))
	);
	
	FEditorPlusUtils::GetLevelEditorModule().GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	ModuleChangedHandler = FModuleManager::Get().OnModulesChanged().AddSP(this, &FEditorPlusMenuBase::OnMenuModuleChanged);

	RegisterPath();
}

void FEditorPlusMenuBase::OnMenuExtension(FMenuBuilder& MenuBuilder)
{
	BuildMenu(MenuBuilder);
}

void FEditorPlusMenuBase::OnMenuModuleChanged(FName ModuleThatChanged, EModuleChangeReason ReasonForChange)
{
	if (ModuleThatChanged == FEditorPlusModule::GetName() && ReasonForChange == EModuleChangeReason::ModuleLoaded)
	{
		FEditorPlusMenuManager::RegisterDelegate(GetUniqueId(), FMenuExtensionDelegate::CreateSP(this, &FEditorPlusMenuBase::OnMenuExtension));
		FEditorPlusMenuManager::RegisterDelegate(AsShared(), GetUniqueId(), FOnGetContent::CreateStatic(&FEditorPlusMenuBase::OnToolBarContentWrapper, AsWeak()));
	}
}

void FEditorPlusMenuBase::UnregisterExtension()
{
	if(MenuExtender.IsValid())
	{
		FEditorPlusMenuManager::RemoveDelegate<FMenuExtensionDelegate>(GetUniqueId());
		FEditorPlusMenuManager::RemoveDelegate<FMenuBarExtensionDelegate>(GetUniqueId());
		FEditorPlusMenuManager::RemoveDelegate<FToolBarExtensionDelegate>(GetUniqueId());
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


void FEditorPlusMenuBase::PreBuildMenuBar(FMenuBarBuilder& MenuBuilder)
{
	for (auto& Subscriber: PreBuildMenuBarSubscribers)
	{
		Subscriber.Get<1>().ExecuteIfBound(MenuBuilder);
	}
}

void FEditorPlusMenuBase::RegisterMenuBarExtension(const FName& ExtensionHook, const EExtensionHook::Position Position)
{
	UnregisterExtension();
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

	RegisterPath();
}

void FEditorPlusMenuBase::OnMenuBarExtension(FMenuBarBuilder& MenuBarBuilder)
{
	BuildMenuBar(MenuBarBuilder);
}

void FEditorPlusMenuBase::OnMenuBarModuleChanged(FName ModuleThatChanged, EModuleChangeReason ReasonForChange)
{
	OnMenuModuleChanged(ModuleThatChanged, ReasonForChange);
}


void FEditorPlusMenuBase::PreBuildToolBar(FToolBarBuilder& ToolBarBuilder)
{
	for (auto& Subscriber: PreBuildToolBarSubscribers)
	{
		Subscriber.Get<1>().ExecuteIfBound(ToolBarBuilder);
	}
}

void FEditorPlusMenuBase::RegisterToolBarExtension(const FName& ExtensionHook, const EExtensionHook::Position Position)
{
	UnregisterExtension();
	MenuExtender = MakeShared<FExtender>();
	MenuExtender->AddToolBarExtension(
		ExtensionHook, Position,
		nullptr,
		FEditorPlusMenuManager::RegisterDelegate(
			GetUniqueId(),
			FToolBarExtensionDelegate::CreateSP(this, &FEditorPlusMenuBase::OnToolBarExtension))
	);

	FEditorPlusUtils::GetLevelEditorModule().GetToolBarExtensibilityManager()->AddExtender(MenuExtender);

	FEditorPlusMenuManager::RegisterDelegate(AsShared(), GetUniqueId(), FOnGetContent::CreateStatic(&FEditorPlusMenuBase::OnToolBarContentWrapper, AsWeak()));

	ModuleChangedHandler = FModuleManager::Get().OnModulesChanged().AddSP(this, &FEditorPlusMenuBase::OnToolBarModuleChanged);

	RegisterPath();
}

void FEditorPlusMenuBase::OnToolBarExtension(FToolBarBuilder& ToolBarBuilder)
{
	BuildToolBar(ToolBarBuilder);
}


TSharedRef<SWidget> FEditorPlusMenuBase::OnToolBarContentWrapper(TWeakPtr<FEditorPlusMenuBase> Owner)
{
	if (Owner.IsValid())
	{
		const auto OwnerPtr = Owner.Pin();
		if(!OwnerPtr->IsDestroyed()) return OwnerPtr->OnToolBarContent();
	}
	return SNullWidget::NullWidget;
}

TSharedRef<SWidget> FEditorPlusMenuBase::OnToolBarContent()
{
	FMenuBuilder MenuBuilder(true, nullptr);
	BuildMenu(MenuBuilder);
	return MenuBuilder.MakeWidget();
}

void FEditorPlusMenuBase::OnToolBarModuleChanged(FName ModuleThatChanged, EModuleChangeReason ReasonForChange)
{
	OnMenuModuleChanged(ModuleThatChanged, ReasonForChange);
}

TSharedPtr<FEditorPlusMenuBase> FEditorPlusMenuBase::CreateByPathName(const FString& PathName, const FText& FriendlyName, const FText& FriendlyTips)
{
	auto TypeAndName = FEditorPlusMenuBase::GetTypeAndNameByPathName(PathName);
	const FName Name = FName(TypeAndName.Get<1>());
	
	switch(TypeAndName.Get<0>())
	{
		case EEditorPlusMenuType::Hook:
			return MakeShared<FEditorPlusHook>(Name);
		case EEditorPlusMenuType::MenuBar:
			return MakeShared<FEditorPlusMenuBar>(Name, Name, FriendlyName, FriendlyTips);
		case EEditorPlusMenuType::SubMenu:
			return MakeShared<FEditorPlusSubMenu>(Name, Name, FriendlyName, FriendlyTips);
		case EEditorPlusMenuType::Section:
			return MakeShared<FEditorPlusSection>(Name, Name, FriendlyName);
		case EEditorPlusMenuType::Separator:
			return MakeShared<FEditorPlusSeparator>(Name);
		case EEditorPlusMenuType::Command:
			return MakeShared<FEditorPlusCommand>(Name, Name, FriendlyName, FriendlyTips);
		case EEditorPlusMenuType::ToolBar:
			return MakeShared<FEditorPlusToolBar>(Name, Name, FriendlyName, FriendlyTips);
		default:
			return nullptr;
	}
}

TSharedPtr<FEditorPlusMenuBase> FEditorPlusMenuBase::AddChildByPath(const FString& InPath, const TSharedPtr<FEditorPlusMenuBase>& InMenu)
{
	return FEditorPlusPath::RegisterChildPath(AsShared(), InPath, InMenu);
}

TSharedPtr<FEditorPlusMenuBase> FEditorPlusMenuBase::AddChildByPath(const FString& InPath, const FText& InFriendlyName, const FText& InFriendlyTips)
{
	return FEditorPlusPath::RegisterChildPath(AsShared(), InPath, InFriendlyName, InFriendlyTips);
}

TSharedPtr<FEditorPlusMenuBase> FEditorPlusMenuBase::AddChildActionByPath(
	const FString& InPath, const FExecuteAction& InExecuteAction, const FName& InHook, const FText& InFriendlyName, const FText& InFriendlyTips)
{
	return FEditorPlusPath::RegisterChildPathAction(AsShared(), InPath, InExecuteAction, InHook, InFriendlyName, InFriendlyTips);
}

//
// endregion FEditorPlusMenuBase
// ---------------------------------------------------------------------


// ---------------------------------------------------------------------
// region FEditorPlusMenuBar
//

void FEditorPlusMenuBar::BuildMenuBar(FMenuBarBuilder& MenuBarBuilder)
{
	PreBuildMenuBar(MenuBarBuilder);
	MenuBarBuilder.AddPullDownMenu(
		GetFriendlyName(),
		GetFriendlyTips(),
		FEditorPlusMenuManager::GetDelegate<FNewMenuDelegate>(GetUniqueId()),
		Hook);
}


void FEditorPlusMenuBar::OnMenuExtension(FMenuBuilder& MenuBuilder)
{
	FEditorPlusMenuBase::BuildMenu(MenuBuilder);
}

//
// endregion FEditorPlusMenuBar
// ---------------------------------------------------------------------


// ---------------------------------------------------------------------
// region FEditorPlusMenuSection
//

void FEditorPlusSection::BuildMenu(FMenuBuilder& MenuBuilder)
{
	PreBuildMenu(MenuBuilder);
	MenuBuilder.BeginSection(Hook, GetFriendlyName());
	for (const auto Menu: Children)
	{
		Menu->BuildMenu(MenuBuilder);
	}
	MenuBuilder.EndSection();
}

//
// endregion FEditorPlusMenuSection
// ---------------------------------------------------------------------

// ---------------------------------------------------------------------
// region FEditorPlusSeparator
//

void FEditorPlusSeparator::BuildMenu(FMenuBuilder& MenuBuilder)
{
	PreBuildMenu(MenuBuilder);
	MenuBuilder.AddMenuSeparator(Hook);
	for (const auto Menu: Children)
	{
		Menu->BuildMenu(MenuBuilder);
	}
}

//
// endregion FEditorPlusSeparator
// ---------------------------------------------------------------------

// ---------------------------------------------------------------------
// region FEditorPlusSubMenu
//

void FEditorPlusSubMenu::BuildMenu(FMenuBuilder& MenuBuilder)
{
	PreBuildMenu(MenuBuilder);
	MenuBuilder.AddSubMenu(
		GetFriendlyName(),
		GetFriendlyTips(),
		FNewMenuDelegate::CreateSP(this, &FEditorPlusSubMenu::MakeSubMenu),
		false,
		FSlateIcon(),
		true,
		Hook
	);
}

void FEditorPlusSubMenu::MakeSubMenu(FMenuBuilder& MenuBuilder)
{
	for (const auto Menu: Children)
	{
		Menu->BuildMenu(MenuBuilder);
	}
}

//
// endregion FEditorPlusSubMenu
// ---------------------------------------------------------------------

// ---------------------------------------------------------------------
// region FEditorPlusCommand
//

TSharedRef<FEditorPlusMenuBase> FEditorPlusCommand::BindCommand(
	const TSharedRef<FUICommandInfo>& InCommandInfo, const TSharedRef<FUICommandList>& InCommandList)
{
	CommandInfo = MakeShared<FEditorPlusCommandInfo>(InCommandInfo, InCommandList);
	return AsShared();
}


TSharedRef<FEditorPlusMenuBase> FEditorPlusCommand::BindAction(
	const FExecuteAction& ExecuteAction,
	const EUserInterfaceActionType& Type,
	const FSlateIcon& Icon)
{
	CommandInfo = MakeShared<FEditorPlusCommandInfo>(ExecuteAction, Type, Icon);
	return AsShared();
}

void FEditorPlusCommand::BuildMenu(FMenuBuilder& MenuBuilder)
{
	PreBuildMenu(MenuBuilder);
	if (!CommandInfo.IsValid()) return;
	
	if (CommandInfo->CommandList.IsValid() && CommandInfo->CommandInfo.IsValid())
	{
		MenuBuilder.PushCommandList(CommandInfo->CommandList.ToSharedRef());
    	MenuBuilder.AddMenuEntry(CommandInfo->CommandInfo, Hook);
    	MenuBuilder.PopCommandList();	
	}
	else
	{
		MenuBuilder.AddMenuEntry(
			GetFriendlyName(), GetFriendlyTips(), CommandInfo->Icon,
			CommandInfo->ExecuteAction, Hook, CommandInfo->Type);
	}
}

//
// endregion FEditorPlusCommand
// ---------------------------------------------------------------------

// ---------------------------------------------------------------------
// region FEditorPlusWidget
//
void FEditorPlusWidget::BuildMenu(FMenuBuilder& MenuBuilder)
{
	PreBuildMenu(MenuBuilder);
	if(Widget.IsValid())
		MenuBuilder.AddWidget(Widget.ToSharedRef(), GetFriendlyName());
}

//
// endregion FEditorPlusWidget
// ---------------------------------------------------------------------

// ---------------------------------------------------------------------
// region FEditorPlusToolBar
//

void FEditorPlusToolBar::BuildToolBar(FToolBarBuilder& ToolBarBuilder)
{
	PreBuildToolBar(ToolBarBuilder);

	ToolBarBuilder.SetLabelVisibility(EVisibility::Visible);
	ToolBarBuilder.AddSeparator(Hook);
	ToolBarBuilder.AddComboButton(
		FUIAction(),
		FEditorPlusMenuManager::GetDelegate<FOnGetContent>(GetUniqueId()),
		GetFriendlyName(),
		GetFriendlyTips(),
		FSlateIcon(),
		false
	);
}

//
// endregion FEditorPlusToolBar
// ---------------------------------------------------------------------
