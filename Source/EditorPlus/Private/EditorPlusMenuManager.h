// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorPlus.h"

template <class DelegateClass, class BuilderClass, class Subsystem>
class TDelegateHolderBase
{
public:
	static TMap<FName, DelegateClass>* DelegateMap()
	{
		return Subsystem::template GetDelegateMap<DelegateClass>();
	}
	
	static DelegateClass Register(const FName& UniqueId, const DelegateClass& Delegate)
	{
		const auto Map = DelegateMap();
		if(!Map) return nullptr;
		
		checkf(!Map->Contains(UniqueId), TEXT("Duplicate Delegate: %s"), ToCStr(UniqueId.ToString()));
		Map->Emplace(UniqueId, Delegate);
		return DelegateClass::CreateStatic(&TDelegateHolderBase::OnApply, UniqueId);
	}

	static DelegateClass Update(const FName& UniqueId, const DelegateClass& Delegate)
	{
		const auto Map = DelegateMap();
		if(!Map) return nullptr;
		Map->Emplace(UniqueId, Delegate);
		return DelegateClass::CreateStatic(&TDelegateHolderBase::OnApply, UniqueId);	
	}
	
	static void OnApply(BuilderClass& MenuBuilder, const FName UniqueId)
	{
		const auto Map = DelegateMap();
		if(!Map) return;
		
		if (!Map->Contains(UniqueId))
		{
			return;
		}
		(*Map)[UniqueId].ExecuteIfBound(MenuBuilder);
	}
	
	static DelegateClass Get(const FName& UniqueId)
	{
		const auto Map = DelegateMap();
		if(!Map) return nullptr;
		
		if (Map->Contains(UniqueId))
		{
			return DelegateClass::CreateStatic(&TDelegateHolderBase::OnApply, UniqueId);
		}
		return nullptr;
	}

	static int32 Remove(const FName& UniqueId)
	{
		const auto Map = DelegateMap();
		if(!Map) return INDEX_NONE;
		
		return Map->Remove(UniqueId);
	}

	static bool Contains(const FName& UniqueId)
	{
		const auto Map = DelegateMap();
		if(!Map) return INDEX_NONE;
		return Map->Contains(UniqueId);
	}

};

class FPathMenuBarExtensionDelegate: public FMenuBarExtensionDelegate
{
public:
	FPathMenuBarExtensionDelegate(const FMenuBarExtensionDelegate& Delegate): FMenuBarExtensionDelegate(Delegate) {}
	FPathMenuBarExtensionDelegate(TYPE_OF_NULLPTR) {}	
};


class FPathMenuExtensionDelegate: public FMenuExtensionDelegate
{
public:
	FPathMenuExtensionDelegate(const FMenuExtensionDelegate& Delegate): FMenuExtensionDelegate(Delegate) {}
	FPathMenuExtensionDelegate(TYPE_OF_NULLPTR) {}
};

template <class DelegateClass, class Subsystem>
class TDelegateHolder
{};


template <class Subsystem>
class TDelegateHolder<FMenuExtensionDelegate, Subsystem>: public TDelegateHolderBase<FMenuExtensionDelegate, FMenuBuilder, Subsystem>
{};

template <class Subsystem>
class TDelegateHolder<FMenuBarExtensionDelegate, Subsystem>: public TDelegateHolderBase<FMenuBarExtensionDelegate, FMenuBarBuilder, Subsystem>
{};

template <class Subsystem>
class TDelegateHolder<FPathMenuBarExtensionDelegate, Subsystem>: public TDelegateHolderBase<FPathMenuBarExtensionDelegate, FMenuBarBuilder, Subsystem>
{};

template <class Subsystem>
class TDelegateHolder<FPathMenuExtensionDelegate, Subsystem>: public TDelegateHolderBase<FPathMenuExtensionDelegate, FMenuBuilder, Subsystem>
{};

template <class DelegateClass>	using TDelegateClassMap = TMap<FName, DelegateClass>;
	
using FDelegateMapTuples = TTuple<
	TDelegateClassMap<FMenuExtensionDelegate>,
	TDelegateClassMap<FMenuBarExtensionDelegate>,
	TDelegateClassMap<FPathMenuBarExtensionDelegate>,
	TDelegateClassMap<FPathMenuExtensionDelegate>
>;

class FEditorPlusMenuBase;
class FEditorPlusPathMenuManager;

class FEditorPlusMenuManager: public TSharedFromThis<FEditorPlusMenuManager>
{
public:
	FEditorPlusMenuManager();

	static TSharedPtr<FEditorPlusMenuManager> Get()
	{
		return FEditorPlusModule::Get().GetMenuManager();
	}
	
	template <class DelegateClass>
	static TMap<FName, DelegateClass>* GetDelegateMap()
	{
		if(!Get().IsValid()) return nullptr;
		return &(Get()->DelegateMap.Get<TDelegateClassMap<DelegateClass>>());
	}

	template <class DelegateClass>
	static DelegateClass RegisterDelegate(const FName& UniqueId, const DelegateClass& Delegate)
	{
		return TDelegateHolder<DelegateClass, FEditorPlusMenuManager>::Register(UniqueId, Delegate);	
	}

	template <class DelegateClass>
	static DelegateClass UpdateDelegate(const FName& UniqueId, const DelegateClass& Delegate)
	{
		return TDelegateHolder<DelegateClass, FEditorPlusMenuManager>::Update(UniqueId, Delegate);	
	}
	
	template <class DelegateClass>
	static DelegateClass GetDelegate(const FName& UniqueId)
	{
		return TDelegateHolder<DelegateClass, FEditorPlusMenuManager>::Get(UniqueId);
	}

	template <class DelegateClass>
	static int32 RemoveDelegate(const FName& UniqueId)
	{
		return TDelegateHolder<DelegateClass, FEditorPlusMenuManager>::Remove(UniqueId);
	}

	template <class DelegateClass>
	static bool ContainsDelegate(const FName& UniqueId)
	{
		return TDelegateHolder<DelegateClass, FEditorPlusMenuManager>::Contains(UniqueId);
	}

	static TSharedPtr<FEditorPlusMenuBase> RegisterPath(const FString& Path, const TSharedPtr<FEditorPlusMenuBase>& Menu=nullptr);
	static TSharedPtr<FEditorPlusMenuBase> RegisterPath(const FString& Path, const FText& FriendlyName, const FText& FriendlyTips);
	static TSharedPtr<FEditorPlusMenuBase> RegisterPathAction(
		const FString& Path, const FExecuteAction& ExecuteAction, const FName& Hook="",
		const FText& FriendlyName=FText::GetEmpty(), const FText& FriendlyTips=FText::GetEmpty());
	static bool UnregisterPath(const FString& Path, const TSharedPtr<FEditorPlusMenuBase>& Leaf=nullptr);
	static bool UnregisterPath(const FString& Path, const FName& UniqueId);
	static TSharedPtr<FEditorPlusMenuBase> GetNodeByPath(const FString& Path);
	
protected:

	FDelegateMapTuples DelegateMap;
	TSharedPtr<FEditorPlusPathMenuManager> PathMenuManager;
};
