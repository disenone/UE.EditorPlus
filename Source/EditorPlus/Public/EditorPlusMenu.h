#pragma once

#include "EditorPlusEnum.h"
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
	
	virtual void Register(FMenuBuilder& MenuBuilder);
	virtual void Register(FMenuBarBuilder& MenuBuilder) {}
	virtual TSharedRef<FEditorPlusMenuBase> RegisterPath();
	virtual void Unregister();
	
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
	
	virtual bool HasMenuExtender() const { return MenuExtender.IsValid(); }
	virtual TSharedRef<FEditorPlusMenuBase> AddExtension(
		const FName& ExtensionHook, const EExtensionHook::Position Position=EExtensionHook::After)
	{
		return AsShared();
	}

	static TSharedPtr<FEditorPlusMenuBase> CreateByPathName(const FString& PathName, const FText& FriendlyName=FText::GetEmpty(), const FText& FriendlyTips=FText::GetEmpty());

	virtual FText GetFriendlyName()
	{
		return FriendlyName;
	}

	virtual FText GetFriendlyTips()
	{
		return FriendlyTips;
	}
protected:
	
	virtual void RemoveMenuExtension();
	virtual void OnMenuExtension(FMenuBuilder& MenuBuilder);
	virtual void OnMenuModuleChanged(FName ModuleThatChanged, EModuleChangeReason ReasonForChange);

	virtual void RemoveMenuBarExtension();
	virtual void OnMenuBarExtension(FMenuBarBuilder& MenuBarBuilder);
	virtual void OnMenuBarModuleChanged(FName ModuleThatChanged, EModuleChangeReason ReasonForChange);
	
	virtual void DoAddMenuExtension(const FName& ExtensionHook, const EExtensionHook::Position Position);
	virtual void DoAddMenuBarExtension(const FName& ExtensionHook, const EExtensionHook::Position Position);

	virtual TSharedRef<FEditorPlusMenuBase> AddMenuExtension(const FName& ExtensionHook, const EExtensionHook::Position Position)
	{
		DoAddMenuExtension(ExtensionHook, Position);
		return AsShared();
	}
	
	virtual TSharedRef<FEditorPlusMenuBase> AddMenuBarExtension(const FName& ExtensionHook, const EExtensionHook::Position Position)
	{
		DoAddMenuBarExtension(ExtensionHook, Position);
		return AsShared();
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
	
	// my extender to hook other menu
	TSharedPtr<FExtender> MenuExtender;

	// callback when module changed (reload)
	FDelegateHandle ModuleChangedHandler;

	// sub menus
	TArray<TSharedRef<FEditorPlusMenuBase>> Children;
};


// Root node, no children
class EDITORPLUS_API TEditorPlusMenuBaseRoot: public FEditorPlusMenuBase
{
public:
	explicit TEditorPlusMenuBaseRoot(
		const FName& InName, const FName& InHook = NAME_None,
		const FText& InFriendlyName=FText::GetEmpty(), const FText& InFriendlyTips=FText::GetEmpty())
		: FEditorPlusMenuBase(InName, InHook, InFriendlyName, InFriendlyTips) {}	

	virtual bool AllowRole(const EEditorPlusMenuRole Role) const override
	{
		return Role._to_integral() == EEditorPlusMenuRole::Root;
	}
};


// Root node, no children
class EDITORPLUS_API TEditorPlusMenuBaseNode: public FEditorPlusMenuBase
{
public:
	explicit TEditorPlusMenuBaseNode(
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
class EDITORPLUS_API TEditorPlusMenuBaseLeaf: public FEditorPlusMenuBase
{
public:
	explicit TEditorPlusMenuBaseLeaf(
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


class EDITORPLUS_API FEditorPlusHook: public TEditorPlusMenuBaseRoot
{
public:
	using FBaseType = TEditorPlusMenuBaseRoot;
	
	explicit FEditorPlusHook(const FName& Name): FBaseType(Name, Name)
	{
		Path = "/" + FEditorPlusHook::GetPathName();
	}

	virtual EEditorPlusMenuType GetType() const override { return StaticType(); }
	static EEditorPlusMenuType StaticType() { return EEditorPlusMenuType::Hook; }

	virtual TSharedRef<FEditorPlusMenuBase> AddExtension(const EExtensionHook::Position Position=EExtensionHook::After)
	{
		return AddExtension(Hook, Position);
	}
	
	virtual TSharedRef<FEditorPlusMenuBase> AddExtension(
		const FName& ExtensionHook, const EExtensionHook::Position Position=EExtensionHook::After) override
	{
		for (const auto Child: Children)
		{
			if(!Child->HasMenuExtender())
				Child->AddExtension(ExtensionHook, Position);
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


class EDITORPLUS_API FEditorPlusMenuBar: public TEditorPlusMenuBaseNode
{
public:
	using FBaseType = TEditorPlusMenuBaseNode;
	
	explicit FEditorPlusMenuBar(
		const FName& InName, const FName& InHook = NAME_None,
		const FText& InFriendlyName=FText::GetEmpty(), const FText& InFriendlyTips=FText::GetEmpty())
		: FBaseType(InName, InHook, InFriendlyName, InFriendlyTips) {}	
		
	virtual ~FEditorPlusMenuBar() override;
	
	virtual void Register(FMenuBarBuilder& MenuBarBuilder) override;

	virtual EEditorPlusMenuType GetType() const override { return StaticType(); }
	static EEditorPlusMenuType StaticType() { return EEditorPlusMenuType::MenuBar; }
	
	using FBaseType::AddMenuBarExtension;
	using FBaseType::RemoveMenuBarExtension;
	virtual TSharedRef<FEditorPlusMenuBase> AddExtension(
		const FName& ExtensionHook, const EExtensionHook::Position Position) override
	{
		return AddMenuBarExtension(ExtensionHook, Position);
	}

	virtual bool AllowChild(const TSharedRef<FEditorPlusMenuBase>& Child) const override
	{
		const auto Type = Child->GetType();
		return Type != EEditorPlusMenuType::_from_integral(EEditorPlusMenuType::None)
			&& Type != EEditorPlusMenuType::_from_integral(EEditorPlusMenuType::Hook)
			&& Type != EEditorPlusMenuType::_from_integral(EEditorPlusMenuType::MenuBar)
		;
	}
protected:
	virtual void OnMenuExtension(FMenuBuilder& MenuBuilder) override;
};


class EDITORPLUS_API FEditorPlusSection: public TEditorPlusMenuBaseNode
{
public:
	using FBaseType = TEditorPlusMenuBaseNode;
	
	explicit FEditorPlusSection(
		const FName& InName, const FName& InHook = NAME_None,
		const FText& InFriendlyName=FText::GetEmpty())
		: FBaseType(InName, InHook, InFriendlyName) {}	
	
	virtual void Register(FMenuBuilder& MenuBuilder) override;

	virtual EEditorPlusMenuType GetType() const override { return StaticType(); }
	static EEditorPlusMenuType StaticType() { return EEditorPlusMenuType::Section; }

	using FBaseType::AddMenuExtension;
	using FBaseType::RemoveMenuExtension;
	virtual TSharedRef<FEditorPlusMenuBase> AddExtension(const FName& ExtensionHook, const EExtensionHook::Position Position) override
	{
		return AddMenuExtension(ExtensionHook, Position);
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
};


class EDITORPLUS_API FEditorPlusSeparator: public TEditorPlusMenuBaseNode
{
public:
	using FBaseType = TEditorPlusMenuBaseNode;
	
	explicit FEditorPlusSeparator(const FName& Hook=NAME_None): FBaseType(Hook, Hook) {}
	
	virtual void Register(FMenuBuilder& MenuBuilder) override;
	
	virtual EEditorPlusMenuType GetType() const override { return StaticType(); }
	static EEditorPlusMenuType StaticType() { return EEditorPlusMenuType::Separator; }
	
	using FBaseType::AddMenuExtension;
	using FBaseType::RemoveMenuExtension;
	virtual TSharedRef<FEditorPlusMenuBase> AddExtension(const FName& ExtensionHook, const EExtensionHook::Position Position) override
	{
		return AddMenuExtension(ExtensionHook, Position);
	}
	
	virtual bool AllowChild(const TSharedRef<FEditorPlusMenuBase>& Child) const override
	{
		const auto Type = Child->GetType();
		return Type != EEditorPlusMenuType::_from_integral(EEditorPlusMenuType::None)
			&& Type != EEditorPlusMenuType::_from_integral(EEditorPlusMenuType::Hook)
			&& Type != EEditorPlusMenuType::_from_integral(EEditorPlusMenuType::MenuBar)
		;
	}
};


class EDITORPLUS_API FEditorPlusSubMenu: public TEditorPlusMenuBaseNode
{
public:
	using FBaseType = TEditorPlusMenuBaseNode;
	
	explicit FEditorPlusSubMenu(
		const FName& InName, const FName& InHook = NAME_None,
		const FText& InFriendlyName=FText::GetEmpty(), const FText& InFriendlyTips=FText::GetEmpty())
		: FBaseType(InName, InHook, InFriendlyName, InFriendlyTips) {}			
		
	virtual void Register(FMenuBuilder& MenuBuilder) override;
	
	virtual EEditorPlusMenuType GetType() const override { return StaticType(); }
	static EEditorPlusMenuType StaticType() { return EEditorPlusMenuType::SubMenu; }
	
	using FBaseType::AddMenuExtension;
	using FBaseType::RemoveMenuExtension;
	virtual TSharedRef<FEditorPlusMenuBase> AddExtension(const FName& ExtensionHook, const EExtensionHook::Position Position) override
	{
		return AddMenuExtension(ExtensionHook, Position);
	}

	virtual bool AllowChild(const TSharedRef<FEditorPlusMenuBase>& Child) const override
	{
		const auto Type = Child->GetType();
		return Type != EEditorPlusMenuType::_from_integral(EEditorPlusMenuType::None)
			&& Type != EEditorPlusMenuType::_from_integral(EEditorPlusMenuType::Hook)
			&& Type != EEditorPlusMenuType::_from_integral(EEditorPlusMenuType::MenuBar)
		;
	}
protected:
	void MakeSubMenu(FMenuBuilder& MenuBuilder);
};


class EDITORPLUS_API FEditorPlusCommand: public TEditorPlusMenuBaseLeaf
{
public:
	using FBaseType = TEditorPlusMenuBaseLeaf;

	explicit FEditorPlusCommand(
		const FName& InName, const FName& InHook = NAME_None,
		const FText& InFriendlyName=FText::GetEmpty(), const FText& InFriendlyTips=FText::GetEmpty())
		: FBaseType(InName, InHook, InFriendlyName, InFriendlyTips) {}
	
	virtual ~FEditorPlusCommand() override;
	
	TSharedRef<FEditorPlusMenuBase> BindAction(
		const FExecuteAction& ExecuteAction,
		const EUserInterfaceActionType& Type = EUserInterfaceActionType::Button,
		const FInputChord& Chord = FInputChord(),
		const FSlateIcon& Icon = FSlateIcon());
	
	TSharedRef<FEditorPlusMenuBase> BindAction(
		const TSharedRef<IEditorPlusCommandsInterface>& CommandMgr,
		const FExecuteAction& ExecuteAction,
		const EUserInterfaceActionType& Type = EUserInterfaceActionType::Button,
		const FInputChord& Chord = FInputChord(),
		const FSlateIcon& Icon = FSlateIcon());
	
	virtual void Register(FMenuBuilder& MenuBuilder) override;
	virtual void Unregister() override;

	virtual EEditorPlusMenuType GetType() const override { return StaticType(); }
	static EEditorPlusMenuType StaticType() { return EEditorPlusMenuType::Command; }

	using FBaseType::AddMenuExtension;
	using FBaseType::RemoveMenuExtension;
	virtual TSharedRef<FEditorPlusMenuBase> AddExtension(const FName& ExtensionHook, const EExtensionHook::Position Position) override
	{
		return AddMenuExtension(ExtensionHook, Position);
	}

	virtual void SetParentPath(const FString& ParentPath) override;

	virtual bool AllowChild(const TSharedRef<FEditorPlusMenuBase>& Child) const override { return false; }
protected:
	
	TSharedPtr<FEditorPlusCommandInfo> CommandInfo;
	TSharedPtr<IEditorPlusCommandsInterface> CommandMgr;
};


class EDITORPLUS_API FEditorPlusWidget: public TEditorPlusMenuBaseLeaf
{
public:
	using FBaseType = TEditorPlusMenuBaseLeaf;

	explicit FEditorPlusWidget(const FName& InName, const FText& InFriendlyName=FText::GetEmpty())
		: FBaseType(InName, NAME_None, InFriendlyName)
	{
		// keep the original FriendlyName, because Widget is not necessary to have one
		FriendlyName = InFriendlyName;
	}	
	
	TSharedRef<FEditorPlusMenuBase> BindWidget(const TSharedRef<SWidget>& _Widget) { this->Widget = _Widget; return AsShared(); }
	
	virtual void Register(FMenuBuilder& MenuBuilder) override;
	
	virtual EEditorPlusMenuType GetType() const override { return StaticType(); }
	static EEditorPlusMenuType StaticType() { return EEditorPlusMenuType::Command; }

	using FBaseType::AddMenuExtension;
	using FBaseType::RemoveMenuExtension;
	virtual TSharedRef<FEditorPlusMenuBase> AddExtension(const FName& ExtensionHook, const EExtensionHook::Position Position) override
	{
		return AddMenuExtension(ExtensionHook, Position);
	}

	virtual bool AllowChild(const TSharedRef<FEditorPlusMenuBase>& Child) const override { return false; }
protected:
	TSharedPtr<SWidget> Widget;
};
