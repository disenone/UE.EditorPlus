// Copyright (c) 2023 - 2024, Disenone (https://github.com/disenone).
// All rights reserved. Licensed under MIT License.

#pragma once

#include "CoreMinimal.h"
#include "EditorPlus.h"
#include "EditorPlusMenu.h"

template <class DelegateClass, class DataHolder, typename DelegateSignature>
class TOnApply
{
};

template <class DelegateClass, class DataHolder, class RetValType, typename... VarTypes>
class TOnApply<DelegateClass, DataHolder, RetValType(VarTypes...)>
{
public:
	static TMap<FName, DelegateClass>* DelegateMap()
	{
		return DataHolder::template GetDelegateMap<DelegateClass>();
	}

	static RetValType OnApply(VarTypes... Vars, const FName UniqueId)
	{
		const auto Map = DelegateMap();
		return (*Map)[UniqueId].Execute(Forward<VarTypes>(Vars)...);
	}

	template<class UserClass, ESPMode Mode>
	static DelegateClass Register(const TSharedRef<UserClass, Mode>& InUserObjectRef, const FName& UniqueId, const DelegateClass& Delegate)
	{
		const auto Map = DelegateMap();
		if(!Map) return nullptr;
		Map->Emplace(UniqueId, Delegate);
		return DelegateClass::CreateSPLambda(InUserObjectRef, &OnApply, UniqueId);
	}
};

template <class DelegateClass, class DataHolder, typename... VarTypes>
class TOnApply<DelegateClass, DataHolder, void(VarTypes...)>
{
public:
	static TMap<FName, DelegateClass>* DelegateMap()
	{
		return DataHolder::template GetDelegateMap<DelegateClass>();
	}

	static void OnApply(VarTypes... Vars, const FName UniqueId)
	{
		const auto Map = DelegateMap();
		if(!Map) return;

		if (!Map->Contains(UniqueId))
		{
			return;
		}
		(*Map)[UniqueId].ExecuteIfBound(Forward<VarTypes>(Vars)...);
	}

	static DelegateClass Register(const FName& UniqueId, const DelegateClass& Delegate)
	{
		const auto Map = DelegateMap();
		if(!Map) return nullptr;

		checkf(!Map->Contains(UniqueId), TEXT("Duplicate Delegate: %s"), ToCStr(UniqueId.ToString()));
		Map->Emplace(UniqueId, Delegate);
		return DelegateClass::CreateStatic(&OnApply, UniqueId);
	}
};


template <class DelegateClass, class DataHolder>
class TDelegateHolderBase: public TOnApply<DelegateClass, DataHolder, typename DelegateClass::TFuncType>
{
	using Super = TOnApply<DelegateClass, DataHolder, typename DelegateClass::TFuncType>;
public:
	using Super::DelegateMap;
	using Super::OnApply;

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


template <class DelegateClass, class Subsystem>
class TDelegateHolder
{};


template <class DataHolder>
class TDelegateHolder<FMenuExtensionDelegate, DataHolder>: public TDelegateHolderBase<FMenuExtensionDelegate, DataHolder>
{};

template <class DataHolder>
class TDelegateHolder<FMenuBarExtensionDelegate, DataHolder>: public TDelegateHolderBase<FMenuBarExtensionDelegate, DataHolder>
{};

template <class DataHolder>
class TDelegateHolder<FToolBarExtensionDelegate, DataHolder>: public TDelegateHolderBase<FToolBarExtensionDelegate, DataHolder>
{};

template <class DataHolder>
class TDelegateHolder<FOnGetContent, DataHolder>: public TDelegateHolderBase<FOnGetContent, DataHolder>
{};

template <class DelegateClass> using TDelegateClassMap = TMap<FName, DelegateClass>;
	
using FDelegateMapTuples = TTuple<
	TDelegateClassMap<FMenuExtensionDelegate>,
	TDelegateClassMap<FMenuBarExtensionDelegate>,
	TDelegateClassMap<FToolBarExtensionDelegate>,
	TDelegateClassMap<FOnGetContent>
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
	static std::enable_if_t<std::is_void_v<typename DelegateClass::RetValType>, DelegateClass> RegisterDelegate(const FName& UniqueId, const DelegateClass& Delegate)
	{
		return TDelegateHolder<DelegateClass, FEditorPlusMenuManager>::Register(UniqueId, Delegate);	
	}

	template <class DelegateClass, class UserClass, ESPMode Mode>
	static std::enable_if_t<!std::is_void_v<typename DelegateClass::RetValType>, DelegateClass> RegisterDelegate(const TSharedRef<UserClass, Mode>& InUserObjectRef, const FName& UniqueId, const DelegateClass& Delegate)
	{
		return TDelegateHolder<DelegateClass, FEditorPlusMenuManager>::Register(InUserObjectRef, UniqueId, Delegate);
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
		const FString& Path, const FExecuteAction& ExecuteAction, const FName& Hook=EP_FNAME_HOOK_AUTO,
		const FText& FriendlyName=FText::GetEmpty(), const FText& FriendlyTips=FText::GetEmpty());
	static TSharedPtr<FEditorPlusMenuBase> RegisterChildPath(
		const TSharedRef<FEditorPlusMenuBase>& InParent, const FString& Path, const TSharedPtr<FEditorPlusMenuBase>& Menu=nullptr);
	static TSharedPtr<FEditorPlusMenuBase> RegisterChildPath(
		const TSharedRef<FEditorPlusMenuBase>& InParent, const FString& Path, const FText& FriendlyName, const FText& FriendlyTips);
	static TSharedPtr<FEditorPlusMenuBase> RegisterChildPathAction(
		const TSharedRef<FEditorPlusMenuBase>& InParent, const FString& Path, const FExecuteAction& ExecuteAction,
		const FName& Hook=EP_FNAME_HOOK_AUTO, const FText& FriendlyName=FText::GetEmpty(), const FText& FriendlyTips=FText::GetEmpty());

	static bool UnregisterPath(const FString& Path, const TSharedPtr<FEditorPlusMenuBase>& Leaf=nullptr);
	static bool UnregisterPath(const FString& Path, const FName& UniqueId);
	static TSharedPtr<FEditorPlusMenuBase> GetNodeByPath(const FString& Path);
	
protected:

	FDelegateMapTuples DelegateMap;
	TSharedPtr<FEditorPlusPathMenuManager> PathMenuManager;
};
