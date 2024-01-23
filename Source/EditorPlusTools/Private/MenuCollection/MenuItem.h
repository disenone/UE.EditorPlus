// Copyright (c) 2024 - 2024, Disenone (https://github.com/disenone).
// All rights reserved. Licensed under MIT License.

#pragma once

#include "EditorPlusUtils.h"

class IMenuItem
{
public:
	enum class EMenuType: uint8
	{
		Command,
		Tab,
	};

	explicit IMenuItem(const EMenuType InType)
		: Type(InType) {}

	virtual ~IMenuItem() = default;

	virtual FName GetName() const = 0;

	virtual FText GetFriendlyName() const = 0;

	virtual FText GetFriendlyTips() const = 0;

	virtual FString GetSearchText() const
	{
		return GetName().ToString() + GetFriendlyName().ToString() + GetFriendlyTips().ToString();
	}

	virtual FSlateIcon GetIcon() const = 0;

	virtual bool RunAction() const = 0;

	virtual FString GetSaveString() const
	{
		return FString::FromInt(static_cast<int32>(Type)) + TEXT(".") + GetName().ToString();
	}

	static TArray<TSharedPtr<IMenuItem>> CollectMenuItems();

	const EMenuType Type;
};


class FMenuItemCommand: public IMenuItem
{
public:
	explicit FMenuItemCommand(const TSharedPtr<const FUICommandInfo>& InCommandInfo, const FUIAction& InAction)
	: IMenuItem(EMenuType::Command), CommandInfo(InCommandInfo), Action(InAction) {}

	virtual FName GetName() const override
	{
		if (CommandInfo.IsValid())
		{
			return CommandInfo.Pin()->GetCommandName();
		}
		return NAME_None;
	}

	virtual FText GetFriendlyName() const override
	{
		if (CommandInfo.IsValid())
		{
			return CommandInfo.Pin()->GetLabel();
		}
		return FText::GetEmpty();
	}

	virtual FText GetFriendlyTips() const override
	{
		if (CommandInfo.IsValid())
		{
			return CommandInfo.Pin()->GetDescription();
		}
		return FText::GetEmpty();
	}

	virtual FSlateIcon GetIcon() const override
	{
		if (CommandInfo.IsValid())
		{
			return CommandInfo.Pin()->GetIcon();
		}
		return FSlateIcon();
	}

	virtual bool RunAction() const override
	{
		if(CommandInfo.IsValid())
		{
			return FEditorPlusUtils::GetLevelEditorModule().GetGlobalLevelEditorActions()->TryExecuteAction(CommandInfo.Pin().ToSharedRef());
		}

		return false;
	}

	const TWeakPtr<const FUICommandInfo> CommandInfo;
	const FUIAction Action;
};


class FMenuItemTab: public IMenuItem
{
public:
	explicit FMenuItemTab(const TSharedPtr<FTabSpawnerEntry>& InTabEntry)
		: IMenuItem(EMenuType::Tab), TabSpawnerEntry(InTabEntry) {}

	virtual FName GetName() const override
	{
		if (TabSpawnerEntry.IsValid())
		{
			return TabSpawnerEntry.Pin()->GetTabType();
		}
		return NAME_None;
	}

	virtual FText GetFriendlyName() const override
	{
		if (TabSpawnerEntry.IsValid())
		{
			return TabSpawnerEntry.Pin()->GetDisplayName();
		}
		return FText::GetEmpty();
	}

	virtual FText GetFriendlyTips() const override
	{
		if (TabSpawnerEntry.IsValid())
		{
			return TabSpawnerEntry.Pin()->GetTooltipText();
		}
		return FText::GetEmpty();
	}

	virtual FSlateIcon GetIcon() const override
	{
		if (TabSpawnerEntry.IsValid())
		{
			return TabSpawnerEntry.Pin()->GetIcon();
		}
		return FSlateIcon();
	}

	virtual bool RunAction() const override
	{
		if(TabSpawnerEntry.IsValid())
		{
			return FGlobalTabmanager::Get()->TryInvokeTab(TabSpawnerEntry.Pin()->GetTabType()).IsValid();
		}

		return false;
	}

	const TWeakPtr<FTabSpawnerEntry> TabSpawnerEntry;
};
