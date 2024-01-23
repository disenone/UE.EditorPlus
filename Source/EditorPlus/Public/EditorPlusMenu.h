// Copyright (c) 2023 - 2024, Disenone (https://github.com/disenone).
// All rights reserved. Licensed under MIT License.

#pragma once

#include "ThirdParty/BetterEnums/Enum.h"
#include "EditorPlusCommandInfo.h"
#include "EditorPlusLog.h"

#define EP_NEW_MENU(Class) MakeShared<Class>
#define EP_FNAME_HOOK_AUTO TEXT("__AUTO__")

BETTER_ENUM(EEditorPlusMenuRole, uint8,
	None,
	Root,
	Node,
	Leaf
);


BETTER_ENUM(EEditorPlusMenuType, uint8,
	None,
	Hook,
	MenuBar,
	Separator,
	Section,
	SubMenu,
	Command,	
	Widget
);

class EDITORPLUS_API FEditorPlusMenuBase: public TSharedFromThis<FEditorPlusMenuBase>
{
public:
	explicit FEditorPlusMenuBase(
		const FName& InName, const FName& InHook=NAME_None,
		const FText& InFriendlyName=FText::GetEmpty(), const FText& InFriendlyTips=FText::GetEmpty())
		: Name(InName), FriendlyName(InFriendlyName), FriendlyTips(InFriendlyTips), Hook(InHook)
	{
		if(FriendlyName.IsEmpty()) FriendlyName = FText::FromName(Name);
	}
	
	virtual ~FEditorPlusMenuBase();
	virtual void Destroy(const bool UnregisterPath=true);
	bool IsDestroyed() const { return Destroyed; }
	
	virtual TArray<TSharedRef<FEditorPlusMenuBase>>::SizeType ChildrenNum() const { return Children.Num(); }
	
	virtual bool IsEmpty() const { return Children.IsEmpty(); }

	virtual bool AllowChild(const TSharedRef<FEditorPlusMenuBase>& Child) const = 0;

	virtual bool HasChild(const TSharedRef<FEditorPlusMenuBase>& Child) const
	{
		return Children.Contains(Child);	
	}
	virtual bool HasChild(const FName& ChildUniqueId) const
	{
		return !GetChildrenByUniqueId(ChildUniqueId).IsEmpty();
	}
	
	virtual bool AddChild(const TSharedRef<FEditorPlusMenuBase>& Child)
	{
		if(!AllowChild(Child))
		{
			UE_LOG(LogEditorPlus, Error, TEXT("Menu [%s] not allow child [%s]."), ToCStr(GetPathName()), ToCStr(Child->GetPathName()));
			return false;
		}
		if(HasChild(Child)) return true;

		Child->SetParentPath(Path);

		TArray<TSharedRef<FEditorPlusMenuBase>> OldChildren;
		// replace same uniqueId
		if(HasChild(Child->GetUniqueId()))
		{
			OldChildren.Append(GetChildrenByUniqueId(Child->GetUniqueId()));
			UE_LOG(
				LogEditorPlus, Error, TEXT("Menu [%s] adding duplicate child [%s], replace to the new one."),
				ToCStr(GetPathName()), ToCStr(Child->GetPathName()));
		}
				
		Children.Emplace(Child);
		for(auto OldChild: OldChildren)
		{
			OldChild->Destroy();
			RemoveChild(OldChild);
		}	
		return true;
	}

	virtual bool AddChildren(const TArray<TSharedRef<FEditorPlusMenuBase>>& Menus)
	{
		for(const auto Child: Menus)
		{
			AddChild(Child);
		}
		return true;
	}
	
	virtual bool RemoveChild(const TSharedRef<FEditorPlusMenuBase>& Child) { return Children.Remove(Child) > 0; }
	virtual bool RemoveChildByPathName(const FString& PathName)
	{
		for(const auto Child: Children)
		{
			if(Child->GetPathName() == PathName)
			{
				return RemoveChild(Child);
			}
		}
		return false;
	}
	virtual bool RemoveChildByUniqueId(const FName& _UniqueId)
	{
		for(const auto Child: Children)
		{
			if(Child->GetUniqueId() == _UniqueId)
			{
				return RemoveChild(Child);
			}
		}
		return false;	
	}
	
	virtual void ClearChildren() { Children.Empty(); }

	TArray<TSharedRef<FEditorPlusMenuBase>> GetChildrenByName(
		const FName& ChildName, const EEditorPlusMenuType Type = EEditorPlusMenuType::None) const
	{
		TArray<TSharedRef<FEditorPlusMenuBase>> Ret;
		for (auto Menu: Children)
		{
			if (Menu->GetName() == ChildName && (Type._to_integral() == EEditorPlusMenuType::None || Menu->IsType(Type)))
			{
				Ret.Push(Menu);
			}
		}
		return Ret;
	}
	
	template <class Derived>
	TArray<TSharedRef<Derived>> GetChildrenByName(const FName& Name) const
	{
		TArray<TSharedRef<FEditorPlusMenuBase>> BaseRet = GetChildrenByName(Name, Derived::StaticType());
		TArray<TSharedRef<Derived>>	Ret;
		Ret.Reserve(BaseRet.Num());
		for(auto Menu: BaseRet)
		{
			Ret.Push(StaticCastSharedRef<Derived>(Menu));
		}
		return Ret;
	}

	TArray<TSharedRef<FEditorPlusMenuBase>> GetChildrenByType(const EEditorPlusMenuType Type) const
	{
		TArray<TSharedRef<FEditorPlusMenuBase>> Ret;
		for (auto Menu: Children)
		{
			if (Menu->IsType(Type))
			{
				Ret.Push(Menu);
			}
		}
		return Ret;	
	}
	
	template <class Derived>
	TArray<TSharedRef<Derived>> GetChildrenByType() const
	{
		TArray<TSharedRef<FEditorPlusMenuBase>> BaseRet = GetChildrenByType(Derived::StaticType());
		TArray<TSharedRef<Derived>>	Ret;
		Ret.Reserve(BaseRet.Num());
		for(auto Menu: BaseRet)
		{
			Ret.Push(StaticCastSharedRef<Derived>(Menu));
		}
		return Ret;
	}

	TArray<TSharedRef<FEditorPlusMenuBase>>	GetChildrenByPathName(const FString& PathName) const
	{
		TArray<TSharedRef<FEditorPlusMenuBase>>	Ret;
		for(auto Menu: Children)
		{
			if(Menu->GetPathName() == PathName)
			{
				Ret.Push(Menu);
			}
		}
		return Ret;	
	}

	TArray<TSharedRef<FEditorPlusMenuBase>>	GetChildrenByUniqueId(const FName& UniqueId) const
	{
		TArray<TSharedRef<FEditorPlusMenuBase>>	Ret;
		for(auto Menu: Children)
		{
			if(Menu->GetUniqueId() == UniqueId)
			{
				Ret.Push(Menu);
			}
		}
		return Ret;	
	}	
	
	virtual TSharedRef<FEditorPlusMenuBase> Content(const TArray<TSharedRef<FEditorPlusMenuBase>>& Menus)
	{
		AddChildren(Menus);
		return AsShared();
	}

	virtual TSharedRef<FEditorPlusMenuBase> ClearContent()
	{
		ClearChildren();
		return AsShared();
	}
	
	virtual TSharedRef<FEditorPlusMenuBase> RegisterPath();

	virtual FName GetName() const { return Name; }
	virtual FName GetUniqueId() const { return FName(Path); }
	virtual FName GetHook() const { return Hook; }
	virtual FName GetExtHook() const { return ExtHook; }
	virtual EEditorPlusMenuType GetType() const = 0;
	template <class Derived>
	bool IsType() const { return GetType() == Derived::StaticType(); }
	bool IsType(const EEditorPlusMenuType& Type) const { return GetType() == Type; }
	virtual TSharedRef<FEditorPlusMenuBase> SetFriendlyName(const FText& InFriendlyName) { FriendlyName = InFriendlyName; return AsShared(); }
	virtual TSharedRef<FEditorPlusMenuBase> SetFriendlyTips(const FText& InFriendlyTips) { FriendlyTips = InFriendlyTips; return AsShared(); }
	virtual FText GetFriendlyName()	{ return FriendlyName; }
	virtual FText GetFriendlyTips()	{ return FriendlyTips; }
	static FString GetTypePathTag(const EEditorPlusMenuType& Type) { return PathTagLeft + FString(Type._to_string()) + PathTagRight; }
	static EEditorPlusMenuType GetPathTagType(FString Tag)
	{
		if(Tag.StartsWith(PathTagLeft) && Tag.EndsWith(PathTagRight))
		{
			Tag = Tag.Mid(1, Tag.Len() - 2);
			
			const auto TypeName = StringCast<ANSICHAR>(*Tag);
			if(!EEditorPlusMenuType::_is_valid_nocase(TypeName.Get()))
				return EEditorPlusMenuType::None;
				
			return EEditorPlusMenuType::_from_string(TypeName.Get());
		}
		return EEditorPlusMenuType::None;
	}
	virtual FString GetPathTag() const { return GetTypePathTag(GetType()); };
	virtual FString GetPathName() { return GetPathTag() + GetName().ToString(); }
	static TTuple<EEditorPlusMenuType, FString> GetTypeAndNameByPathName(const FString& PathName)
	{
		const auto Index = PathName.Find(PathTagRight);
		if(PathName.StartsWith(PathTagLeft) && Index != INDEX_NONE)
		{
			const FString PathTag = PathName.Left(Index + 1);
			const FString Name = PathName.Right(PathName.Len() - Index - 1);
			return TTuple<EEditorPlusMenuType, FString>(GetPathTagType(PathTag), Name);
		}
		return TTuple<EEditorPlusMenuType, FString>(EEditorPlusMenuType::None, PathName);
	}
	virtual void SetParentPath(const FString& ParentPath)
	{
		Path = ParentPath / GetPathName();
		if (!Children.IsEmpty())
		{
			for(const auto Child: Children)
			{
				Child->SetParentPath(Path);
			}
		}
	}
	virtual FString GetPath() { if (Path.IsEmpty()) Path = GetPathName(); return Path; }
	
	virtual bool AllowRole(const EEditorPlusMenuRole Role) const { return false; }
	
	virtual bool IsExtensionRegistered() const { return MenuExtender.IsValid(); }
	virtual TSharedRef<FEditorPlusMenuBase> RegisterExtension(
		const FName& ExtensionHook, const EExtensionHook::Position Position=EExtensionHook::After)	{ return AsShared(); }
	virtual void UnregisterExtension();

	static TSharedPtr<FEditorPlusMenuBase> CreateByPathName(
		const FString& PathName, const FText& FriendlyName=FText::GetEmpty(), const FText& FriendlyTips=FText::GetEmpty());

	virtual FDelegateHandle SubscribePreBuildMenu(const FMenuExtensionDelegate& MenuExtensionDelegate) { return FDelegateHandle(); }
	virtual FDelegateHandle SubscribePreBuildMenuBar(const FMenuBarExtensionDelegate& MenuBarExtensionDelegate) { return FDelegateHandle(); }
	virtual void UnsubscribePreBuildMenu(const FDelegateHandle& InHandler) { if(PreBuildMenuSubscribers.Contains(InHandler)) PreBuildMenuSubscribers.Remove(InHandler); }
	virtual void UnsubscribePreBuildMenuBar(const FDelegateHandle& InHandler) { if(PreBuildMenuBarSubscribers.Contains(InHandler)) PreBuildMenuBarSubscribers.Remove(InHandler); }

	virtual void PreBuildMenu(FMenuBuilder& MenuBuilder);
	virtual void BuildMenu(FMenuBuilder& MenuBuilder);
	virtual void PreBuildMenuBar(FMenuBarBuilder& MenuBuilder);
	virtual void BuildMenuBar(FMenuBarBuilder& MenuBuilder) {}

protected:

	virtual void RegisterMenuExtension(const FName& ExtensionHook, const EExtensionHook::Position Position);
	virtual void OnMenuExtension(FMenuBuilder& MenuBuilder);
	virtual void OnMenuModuleChanged(FName ModuleThatChanged, EModuleChangeReason ReasonForChange);
	virtual FDelegateHandle DoSubscribePreBuildMenu(const FMenuExtensionDelegate& MenuExtensionDelegate)
	{
		const auto Handler = FDelegateHandle(FDelegateHandle::GenerateNewHandle);
		PreBuildMenuSubscribers.Add(Handler, MenuExtensionDelegate);
		return Handler;
	}

	virtual void RegisterMenuBarExtension(const FName& ExtensionHook, const EExtensionHook::Position Position);
	virtual void OnMenuBarExtension(FMenuBarBuilder& MenuBarBuilder);
	virtual void OnMenuBarModuleChanged(FName ModuleThatChanged, EModuleChangeReason ReasonForChange);
	virtual FDelegateHandle DoSubscribePreBuildMenuBar(const FMenuBarExtensionDelegate& MenuBarExtensionDelegate)
	{
		const auto Handler = FDelegateHandle(FDelegateHandle::GenerateNewHandle);
		PreBuildMenuBarSubscribers.Add(Handler, MenuBarExtensionDelegate);
		return Handler;
	}

public:
	inline static const FString PathTagLeft = "<";
	inline static const FString PathTagRight = ">";
	
protected:
	// my name, will use for path
	FName Name = "";

	// my friendly name, will show in menu
	FText FriendlyName = FText::GetEmpty();
	
	// tips for menu
	FText FriendlyTips = FText::GetEmpty();
	
	// hook for other menu to extend
	FName Hook = NAME_None;

	// hook to extend other menu
	FName ExtHook = NAME_None;

	// Path to me
	FString Path;

	// is destroyed
	bool Destroyed = false;
	
	// my extender to hook editor
	TSharedPtr<FExtender> MenuExtender;

	// callback when module changed (reload)
	FDelegateHandle ModuleChangedHandler;

	// sub menus
	TArray<TSharedRef<FEditorPlusMenuBase>> Children;

	// subscribe delegate
	TMap<FDelegateHandle, FMenuExtensionDelegate> PreBuildMenuSubscribers;
	TMap<FDelegateHandle, FMenuBarExtensionDelegate> PreBuildMenuBarSubscribers;
};


// Root node, no children
class EDITORPLUS_API FEditorPlusMenuBaseRoot: public FEditorPlusMenuBase
{
public:
	explicit FEditorPlusMenuBaseRoot(
		const FName& InName, const FName& InHook = NAME_None,
		const FText& InFriendlyName=FText::GetEmpty(), const FText& InFriendlyTips=FText::GetEmpty())
		: FEditorPlusMenuBase(InName, InHook, InFriendlyName, InFriendlyTips) {}	

	virtual bool AllowRole(const EEditorPlusMenuRole Role) const override
	{
		return Role._to_integral() == EEditorPlusMenuRole::Root;
	}
};


// Root node, no children
class EDITORPLUS_API FEditorPlusMenuBaseNode: public FEditorPlusMenuBase
{
public:
	explicit FEditorPlusMenuBaseNode(
		const FName& InName, const FName& InHook = NAME_None,
		const FText& InFriendlyName=FText::GetEmpty(), const FText& InFriendlyTips=FText::GetEmpty())
		: FEditorPlusMenuBase(InName, InHook, InFriendlyName, InFriendlyTips) {}	
	virtual bool AllowRole(const EEditorPlusMenuRole Role) const override
	{
		return Role._to_integral() == EEditorPlusMenuRole::Node
			|| Role._to_integral() == EEditorPlusMenuRole::Leaf;
	}
};


// Leaf node, no children
class EDITORPLUS_API FEditorPlusMenuBaseLeaf: public FEditorPlusMenuBase
{
public:
	explicit FEditorPlusMenuBaseLeaf(
		const FName& InName, const FName& InHook = NAME_None,
		const FText& InFriendlyName=FText::GetEmpty(), const FText& InFriendlyTips=FText::GetEmpty())
		: FEditorPlusMenuBase(InName, InHook, InFriendlyName, InFriendlyTips) {}	
	
	virtual bool AllowRole(const EEditorPlusMenuRole Role) const override
	{
		return Role == EEditorPlusMenuRole(EEditorPlusMenuRole::Leaf);
	}

private:
	using FEditorPlusMenuBase::Content;
	using FEditorPlusMenuBase::ClearContent;
	using FEditorPlusMenuBase::AddChildren;
	using FEditorPlusMenuBase::ClearChildren;
};


class EDITORPLUS_API FEditorPlusHook: public FEditorPlusMenuBaseRoot
{
public:
	using FBaseType = FEditorPlusMenuBaseRoot;
	
	explicit FEditorPlusHook(const FName& Name): FBaseType(Name, Name)
	{
		Path = "/" + FEditorPlusHook::GetPathName();
	}

	virtual EEditorPlusMenuType GetType() const override { return StaticType(); }
	static EEditorPlusMenuType StaticType() { return EEditorPlusMenuType::Hook; }

	virtual TSharedRef<FEditorPlusMenuBase> RegisterExtension(const EExtensionHook::Position Position=EExtensionHook::After)
	{
		return RegisterExtension(Hook, Position);
	}

	virtual TSharedRef<FEditorPlusMenuBase> RegisterExtension(
		const FName& ExtensionHook, const EExtensionHook::Position Position=EExtensionHook::After) override
	{
		for (const auto Child: Children)
		{
			if(!Child->IsExtensionRegistered())
				Child->RegisterExtension(ExtensionHook, Position);
		}
		return AsShared();
	}

	virtual bool AllowChild(const TSharedRef<FEditorPlusMenuBase>& Child) const override
	{
		const auto Type = Child->GetType();
		return Type != EEditorPlusMenuType::_from_integral(EEditorPlusMenuType::None)
			&& Type != EEditorPlusMenuType::_from_integral(EEditorPlusMenuType::Hook)
		;
	}
};


class EDITORPLUS_API FEditorPlusMenuBar: public FEditorPlusMenuBaseNode
{
public:
	using FBaseType = FEditorPlusMenuBaseNode;
	
	explicit FEditorPlusMenuBar(
		const FName& InName, const FName& InHook = NAME_None,
		const FText& InFriendlyName=FText::GetEmpty(), const FText& InFriendlyTips=FText::GetEmpty())
		: FBaseType(InName, InHook, InFriendlyName, InFriendlyTips) {}	
		
	virtual void BuildMenuBar(FMenuBarBuilder& MenuBarBuilder) override;

	virtual EEditorPlusMenuType GetType() const override { return StaticType(); }
	static EEditorPlusMenuType StaticType() { return EEditorPlusMenuType::MenuBar; }
	
	virtual TSharedRef<FEditorPlusMenuBase> RegisterExtension(
		const FName& ExtensionHook, const EExtensionHook::Position Position) override
	{
		RegisterMenuBarExtension(ExtensionHook, Position);
		return AsShared();
	}

	virtual bool AllowChild(const TSharedRef<FEditorPlusMenuBase>& Child) const override
	{
		const auto Type = Child->GetType();
		return Type != EEditorPlusMenuType::_from_integral(EEditorPlusMenuType::None)
			&& Type != EEditorPlusMenuType::_from_integral(EEditorPlusMenuType::Hook)
			&& Type != EEditorPlusMenuType::_from_integral(EEditorPlusMenuType::MenuBar)
		;
	}

	virtual FDelegateHandle SubscribePreBuildMenuBar(const FMenuBarExtensionDelegate& MenuExtensionDelegate) override
	{
		return DoSubscribePreBuildMenuBar(MenuExtensionDelegate);
	}

protected:
	virtual void OnMenuExtension(FMenuBuilder& MenuBuilder) override;
};


class EDITORPLUS_API FEditorPlusSection: public FEditorPlusMenuBaseNode
{
public:
	using FBaseType = FEditorPlusMenuBaseNode;
	
	explicit FEditorPlusSection(
		const FName& InName, const FName& InHook = NAME_None,
		const FText& InFriendlyName=FText::GetEmpty())
		: FBaseType(InName, InHook, InFriendlyName) {}	
	
	virtual void BuildMenu(FMenuBuilder& MenuBuilder) override;

	virtual EEditorPlusMenuType GetType() const override { return StaticType(); }
	static EEditorPlusMenuType StaticType() { return EEditorPlusMenuType::Section; }

	virtual TSharedRef<FEditorPlusMenuBase> RegisterExtension(const FName& ExtensionHook, const EExtensionHook::Position Position) override
	{
		RegisterMenuExtension(ExtensionHook, Position);
		return AsShared();
	}

	virtual bool AllowChild(const TSharedRef<FEditorPlusMenuBase>& Child) const override
	{
		const auto Type = Child->GetType();
		return Type != EEditorPlusMenuType::_from_integral(EEditorPlusMenuType::None)
			&& Type != EEditorPlusMenuType::_from_integral(EEditorPlusMenuType::Hook)
			&& Type != EEditorPlusMenuType::_from_integral(EEditorPlusMenuType::MenuBar)
			&& Type != EEditorPlusMenuType::_from_integral(EEditorPlusMenuType::Section)
		;
	}

	virtual FDelegateHandle SubscribePreBuildMenu(const FMenuExtensionDelegate& MenuExtensionDelegate) override
	{
		return DoSubscribePreBuildMenu(MenuExtensionDelegate);
	}
};


class EDITORPLUS_API FEditorPlusSeparator: public FEditorPlusMenuBaseNode
{
public:
	using FBaseType = FEditorPlusMenuBaseNode;
	
	explicit FEditorPlusSeparator(const FName& Hook=NAME_None): FBaseType(Hook, Hook) {}
	
	virtual void BuildMenu(FMenuBuilder& MenuBuilder) override;
	
	virtual EEditorPlusMenuType GetType() const override { return StaticType(); }
	static EEditorPlusMenuType StaticType() { return EEditorPlusMenuType::Separator; }
	
	virtual TSharedRef<FEditorPlusMenuBase> RegisterExtension(const FName& ExtensionHook, const EExtensionHook::Position Position) override
	{
		RegisterMenuExtension(ExtensionHook, Position);
		return AsShared();
	}

	virtual bool AllowChild(const TSharedRef<FEditorPlusMenuBase>& Child) const override
	{
		const auto Type = Child->GetType();
		return Type != EEditorPlusMenuType::_from_integral(EEditorPlusMenuType::None)
			&& Type != EEditorPlusMenuType::_from_integral(EEditorPlusMenuType::Hook)
			&& Type != EEditorPlusMenuType::_from_integral(EEditorPlusMenuType::MenuBar)
		;
	}

	virtual FDelegateHandle SubscribePreBuildMenu(const FMenuExtensionDelegate& MenuExtensionDelegate) override
	{
		return DoSubscribePreBuildMenu(MenuExtensionDelegate);
	}
};


class EDITORPLUS_API FEditorPlusSubMenu: public FEditorPlusMenuBaseNode
{
public:
	using FBaseType = FEditorPlusMenuBaseNode;
	
	explicit FEditorPlusSubMenu(
		const FName& InName, const FName& InHook = NAME_None,
		const FText& InFriendlyName=FText::GetEmpty(), const FText& InFriendlyTips=FText::GetEmpty())
		: FBaseType(InName, InHook, InFriendlyName, InFriendlyTips) {}			
		
	virtual void BuildMenu(FMenuBuilder& MenuBuilder) override;
	
	virtual EEditorPlusMenuType GetType() const override { return StaticType(); }
	static EEditorPlusMenuType StaticType() { return EEditorPlusMenuType::SubMenu; }
	
	virtual TSharedRef<FEditorPlusMenuBase> RegisterExtension(const FName& ExtensionHook, const EExtensionHook::Position Position) override
	{
		RegisterMenuExtension(ExtensionHook, Position);
		return AsShared();
	}

	virtual bool AllowChild(const TSharedRef<FEditorPlusMenuBase>& Child) const override
	{
		const auto Type = Child->GetType();
		return Type != EEditorPlusMenuType::_from_integral(EEditorPlusMenuType::None)
			&& Type != EEditorPlusMenuType::_from_integral(EEditorPlusMenuType::Hook)
			&& Type != EEditorPlusMenuType::_from_integral(EEditorPlusMenuType::MenuBar)
		;
	}

	virtual FDelegateHandle SubscribePreBuildMenu(const FMenuExtensionDelegate& MenuExtensionDelegate) override
	{
		return DoSubscribePreBuildMenu(MenuExtensionDelegate);
	}
protected:
	virtual void MakeSubMenu(FMenuBuilder& MenuBuilder);
};


class EDITORPLUS_API FEditorPlusCommand: public FEditorPlusMenuBaseLeaf
{
public:
	using FBaseType = FEditorPlusMenuBaseLeaf;

	explicit FEditorPlusCommand(
		const FName& InName, const FName& InHook = NAME_None,
		const FText& InFriendlyName=FText::GetEmpty(), const FText& InFriendlyTips=FText::GetEmpty())
		: FBaseType(InName, InHook, InFriendlyName, InFriendlyTips) {}
	
	TSharedRef<FEditorPlusMenuBase> BindAction(
		const FExecuteAction& ExecuteAction,
		const EUserInterfaceActionType& Type = EUserInterfaceActionType::Button,
		const FSlateIcon& Icon = FSlateIcon());
	
	TSharedRef<FEditorPlusMenuBase> BindCommand(
		const TSharedRef<FUICommandInfo>& InCommandInfo,
		const TSharedRef<FUICommandList>& InCommandList);
	
	virtual void BuildMenu(FMenuBuilder& MenuBuilder) override;

	virtual EEditorPlusMenuType GetType() const override { return StaticType(); }
	static EEditorPlusMenuType StaticType() { return EEditorPlusMenuType::Command; }

	virtual TSharedRef<FEditorPlusMenuBase> RegisterExtension(const FName& ExtensionHook, const EExtensionHook::Position Position) override
	{
		RegisterMenuExtension(ExtensionHook, Position);
		return AsShared();
	}

	virtual bool AllowChild(const TSharedRef<FEditorPlusMenuBase>& Child) const override { return false; }

	virtual FDelegateHandle SubscribePreBuildMenu(const FMenuExtensionDelegate& MenuExtensionDelegate) override
	{
		return DoSubscribePreBuildMenu(MenuExtensionDelegate);
	}
protected:
	
	TSharedPtr<FEditorPlusCommandInfo> CommandInfo;
};


class EDITORPLUS_API FEditorPlusWidget: public FEditorPlusMenuBaseLeaf
{
public:
	using FBaseType = FEditorPlusMenuBaseLeaf;

	explicit FEditorPlusWidget(const FName& InName, const FText& InFriendlyName=FText::GetEmpty())
		: FBaseType(InName, NAME_None, InFriendlyName)
	{
		// keep the original FriendlyName, because Widget is not necessary to have one
		FriendlyName = InFriendlyName;
	}	
	
	TSharedRef<FEditorPlusMenuBase> BindWidget(const TSharedRef<SWidget>& _Widget) { this->Widget = _Widget; return AsShared(); }
	
	virtual void BuildMenu(FMenuBuilder& MenuBuilder) override;
	
	virtual EEditorPlusMenuType GetType() const override { return StaticType(); }
	static EEditorPlusMenuType StaticType() { return EEditorPlusMenuType::Command; }

	virtual TSharedRef<FEditorPlusMenuBase> RegisterExtension(const FName& ExtensionHook, const EExtensionHook::Position Position) override
	{
		RegisterMenuExtension(ExtensionHook, Position);
		return AsShared();
	}

	virtual bool AllowChild(const TSharedRef<FEditorPlusMenuBase>& Child) const override { return false; }

	virtual FDelegateHandle SubscribePreBuildMenu(const FMenuExtensionDelegate& MenuExtensionDelegate) override
	{
		return DoSubscribePreBuildMenu(MenuExtensionDelegate);
	}
protected:
	TSharedPtr<SWidget> Widget;
};
