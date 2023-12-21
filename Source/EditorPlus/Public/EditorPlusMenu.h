#pragma once

#include "EditorPlusEnum.h"
#include "EditorPlusToolInterface.h"
#include "EditorPlusCommandInfo.h"
#include "EditorPlusUtils.h"
#include "EditorPlusLog.h"

#define NEW_EP_MENU(Class) MakeShared<Class>

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
	Widget,
	MenuTool
);

class EDITORPLUS_API FEditorPlusMenuBase: public TSharedFromThis<FEditorPlusMenuBase>
{
public:
	explicit FEditorPlusMenuBase(const FName& Name, const FName& Tips = NAME_None, const FName& Hook = NAME_None)
		:Name(Name), Tips(Tips), Hook(Hook)
	{
		if(Tips == NAME_None) this->Tips = Name;
	}
	
	virtual ~FEditorPlusMenuBase();
	virtual void Destroy(const bool UnregisterPath=true);
	bool IsDestroyed() const { return Destroyed; }
	
	virtual TArray<TSharedRef<FEditorPlusMenuBase>>::SizeType ChildrenNum() const { return Children.Num(); }
	
	virtual bool IsEmpty() const { return Children.IsEmpty(); }

	virtual bool AllowChild() const { return true; }

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
		if(!AllowChild())
		{
			UE_LOG(LogEditorPlus, Error, TEXT("Menu [%s] not allow child."), *GetName().ToString());
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
		if(!AllowChild())
		{
			UE_LOG(LogEditorPlus, Error, TEXT("Menu [%s] not allow children."), *GetName().ToString());
			return false;
		}
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

	virtual void RegisterWithMerge(FMenuBuilder& MenuBuilder, const bool bMerge) { Register(MenuBuilder); }
	
	virtual void Unregister();
	
	virtual FName GetName() const { return Name; }
	virtual FName GetUniqueId() const { return FName(Path); }
	virtual FName GetTips() const { return Tips; }
	virtual FName GetHook() const { return Hook; }
	virtual FName GetExtHook() const { return ExtHook; }
	virtual EEditorPlusMenuType GetType() const = 0;
	template <class Derived>
	bool IsType() const { return GetType() == Derived::StaticType(); }
	bool IsType(const EEditorPlusMenuType& Type) const { return GetType() == Type; }

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
	void SetParentPath(const FString& ParentPath)
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
	
	virtual bool AllowRole(const EEditorPlusMenuRole Role) const { return false; }
	
	virtual bool HasMenuExtender() const { return MenuExtender.IsValid(); }
	virtual TSharedRef<FEditorPlusMenuBase> AddExtension(
		const FName& ExtensionHook, const EExtensionHook::Position Position=EExtensionHook::After)
	{
		return AsShared();
	}

	static TSharedPtr<FEditorPlusMenuBase> CreateByPathName(const FString& PathName);

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
	// my name, will show in menu
	FName Name = "";

	// tips for menu
	FName Tips = "";
	
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


template <class Derived>
class EDITORPLUS_API TEditorPlusMenuBase: public FEditorPlusMenuBase 
{
public:
	explicit TEditorPlusMenuBase(const FName& Name, const FName& Tips = NAME_None, const FName& Hook = NAME_None)
		: FEditorPlusMenuBase(Name, Tips, Hook) {}
};


// Root node, no children
template <class Derived>
class EDITORPLUS_API TEditorPlusMenuBaseRoot: public TEditorPlusMenuBase<Derived>
{
public:
	explicit TEditorPlusMenuBaseRoot(const FName& Name, const FName& Tips = NAME_None, const FName& Hook = NAME_None)
		: TEditorPlusMenuBase<Derived>(Name, Tips, Hook) {}	

	virtual bool AllowRole(const EEditorPlusMenuRole Role) const override
	{
		return Role._to_integral() == EEditorPlusMenuRole::Root;
	}
};


// Root node, no children
template <class Derived>
class EDITORPLUS_API TEditorPlusMenuBaseNode: public TEditorPlusMenuBase<Derived>
{
public:
	explicit TEditorPlusMenuBaseNode(const FName& Name, const FName& Tips = NAME_None, const FName& Hook = NAME_None)
		: TEditorPlusMenuBase<Derived>(Name, Tips, Hook) {}	
	virtual bool AllowRole(const EEditorPlusMenuRole Role) const override
	{
		return Role._to_integral() == EEditorPlusMenuRole::Node
			|| Role._to_integral() == EEditorPlusMenuRole::Leaf;
	}
};


// Leaf node, no children
template <class Derived>
class EDITORPLUS_API TEditorPlusMenuBaseLeaf: public TEditorPlusMenuBase<Derived>
{
public:
	explicit TEditorPlusMenuBaseLeaf(const FName& Name, const FName& Tips = NAME_None, const FName& Hook = NAME_None)
		: TEditorPlusMenuBase<Derived>(Name, Tips, Hook) {}	
	
	virtual bool AllowRole(const EEditorPlusMenuRole Role) const override
	{
		return Role == EEditorPlusMenuRole(EEditorPlusMenuRole::Leaf);
	}
	
private:
	using TEditorPlusMenuBase<Derived>::Content;
	using TEditorPlusMenuBase<Derived>::ClearContent;
	using TEditorPlusMenuBase<Derived>::AddChildren;
	using TEditorPlusMenuBase<Derived>::ClearChildren;
};


class EDITORPLUS_API FEditorPlusHook: public TEditorPlusMenuBaseRoot<FEditorPlusHook>
{
public:
	using FBaseType = TEditorPlusMenuBaseRoot<FEditorPlusHook>;
	
	explicit FEditorPlusHook(const FName& Name): FBaseType(Name, Name, Name)
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
};


class EDITORPLUS_API FEditorPlusMenuBar: public TEditorPlusMenuBaseNode<FEditorPlusMenuBar>
{
public:
	using FBaseType = TEditorPlusMenuBaseNode<FEditorPlusMenuBar>;
	
	explicit FEditorPlusMenuBar(const FName& Name, const FName& Tips = NAME_None, const FName& Hook = NAME_None)
		: FBaseType(Name, Tips, Hook) {}
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
	
protected:
	virtual void OnMenuExtension(FMenuBuilder& MenuBuilder) override;
};


class EDITORPLUS_API FEditorPlusSection: public TEditorPlusMenuBaseNode<FEditorPlusSection>
{
public:
	using FBaseType = TEditorPlusMenuBaseNode<FEditorPlusSection>;
	
	explicit FEditorPlusSection(const FName& Name="", const FName& Tips="", const FName& Hook=NAME_None)
		: FBaseType(Name, Tips, Hook)  {}
	
	virtual void Register(FMenuBuilder& MenuBuilder) override;

	virtual EEditorPlusMenuType GetType() const override { return StaticType(); }
	static EEditorPlusMenuType StaticType() { return EEditorPlusMenuType::Section; }

	using FBaseType::AddMenuExtension;
	using FBaseType::RemoveMenuExtension;
	virtual TSharedRef<FEditorPlusMenuBase> AddExtension(const FName& ExtensionHook, const EExtensionHook::Position Position) override
	{
		return AddMenuExtension(ExtensionHook, Position);
	}
};


class EDITORPLUS_API FEditorPlusSeparator: public TEditorPlusMenuBaseNode<FEditorPlusSeparator>
{
public:
	using FBaseType = TEditorPlusMenuBaseNode<FEditorPlusSeparator>;
	
	explicit FEditorPlusSeparator(const FName& Hook=NAME_None): FBaseType(Hook, Hook, Hook) {}
	
	virtual void Register(FMenuBuilder& MenuBuilder) override;
	
	virtual EEditorPlusMenuType GetType() const override { return StaticType(); }
	static EEditorPlusMenuType StaticType() { return EEditorPlusMenuType::Separator; }
	
	using FBaseType::AddMenuExtension;
	using FBaseType::RemoveMenuExtension;
	virtual TSharedRef<FEditorPlusMenuBase> AddExtension(const FName& ExtensionHook, const EExtensionHook::Position Position) override
	{
		return AddMenuExtension(ExtensionHook, Position);
	}
};


class EDITORPLUS_API FEditorPlusSubMenu: public TEditorPlusMenuBaseNode<FEditorPlusSubMenu>
{
public:
	using FBaseType = TEditorPlusMenuBaseNode<FEditorPlusSubMenu>;
	
	explicit FEditorPlusSubMenu(const FName& Name, const FName& Tips="", const FName& Hook=NAME_None)
		: FBaseType(Name, Tips, Hook) {}
	
	virtual void Register(FMenuBuilder& MenuBuilder) override;
	
	virtual EEditorPlusMenuType GetType() const override { return StaticType(); }
	static EEditorPlusMenuType StaticType() { return EEditorPlusMenuType::SubMenu; }
	
	using FBaseType::AddMenuExtension;
	using FBaseType::RemoveMenuExtension;
	virtual TSharedRef<FEditorPlusMenuBase> AddExtension(const FName& ExtensionHook, const EExtensionHook::Position Position) override
	{
		return AddMenuExtension(ExtensionHook, Position);
	}
protected:
	void MakeSubMenu(FMenuBuilder& MenuBuilder);
};


class EDITORPLUS_API FEditorPlusCommand: public TEditorPlusMenuBaseLeaf<FEditorPlusCommand>
{
public:
	using FBaseType = TEditorPlusMenuBaseLeaf<FEditorPlusCommand>;

	explicit FEditorPlusCommand(const FName& Name, const FName& Tips="", const FName& Hook=NAME_None)
		: FBaseType(Name, Tips, Hook) {}
	virtual ~FEditorPlusCommand() override;
	
	TSharedRef<FEditorPlusMenuBase> BindAction(
		const FExecuteAction& ExecuteAction,
		const FName& FriendlyName=NAME_None,
		const EUserInterfaceActionType& Type = EUserInterfaceActionType::Button,
		const FInputChord& Chord = FInputChord(),
		const FName& LoctextNamespace = NAME_None,
		const FSlateIcon& Icon = FSlateIcon());
	
	TSharedRef<FEditorPlusMenuBase> BindAction(
		const TSharedRef<IEditorPlusCommandsInterface>& CommandMgr,
		const FExecuteAction& ExecuteAction,
		const FName& FriendlyName=NAME_None,
		const EUserInterfaceActionType& Type = EUserInterfaceActionType::Button,
		const FInputChord& Chord = FInputChord(),
		const FName& LoctextNamespace = NAME_None,
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

protected:
	
	TSharedPtr<FEditorPlusCommandInfo> CommandInfo;
	TSharedPtr<IEditorPlusCommandsInterface> CommandMgr;
};


class EDITORPLUS_API FEditorPlusWidget: public TEditorPlusMenuBaseLeaf<FEditorPlusWidget>
{
public:
	using FBaseType = TEditorPlusMenuBaseLeaf<FEditorPlusWidget>;

	explicit FEditorPlusWidget(const FName& Name, const FName& Tips="", const FName& Hook=NAME_None)
		: FBaseType(Name, Tips, Hook) {}	
	
	void BindWidget(const TSharedRef<SWidget>& _Widget) { this->Widget = _Widget; }
	
	virtual void Register(FMenuBuilder& MenuBuilder) override
	{
		if(Widget.IsValid())
			MenuBuilder.AddWidget(Widget.ToSharedRef(), FText::FromString(""));
	}
	
	virtual EEditorPlusMenuType GetType() const override { return StaticType(); }
	static EEditorPlusMenuType StaticType() { return EEditorPlusMenuType::Command; }

	using FBaseType::AddMenuExtension;
	using FBaseType::RemoveMenuExtension;
	virtual TSharedRef<FEditorPlusMenuBase> AddExtension(const FName& ExtensionHook, const EExtensionHook::Position Position) override
	{
		return AddMenuExtension(ExtensionHook, Position);
	}
	
protected:
	TSharedPtr<SWidget> Widget;
};


class EDITORPLUS_API FEditorPlusMenuTool: public TEditorPlusMenuBaseLeaf<FEditorPlusMenuTool>
{
public:
	using FBaseType = TEditorPlusMenuBaseLeaf<FEditorPlusMenuTool>;
	
	explicit FEditorPlusMenuTool(const FName& Name, const TSharedRef<IEditorPlusToolInterface>& Tool)
		: FBaseType(Name), Tool(Tool) {}
	virtual ~FEditorPlusMenuTool() override { FEditorPlusMenuTool::Unregister(); }
	
	virtual void Register(FMenuBuilder& MenuBuilder) override
	{
		Tool->OnBuildMenu(MenuBuilder);
	}
	virtual void Unregister() override
	{
		Tool->OnDestroyMenu();
	}

	virtual EEditorPlusMenuType GetType() const override { return StaticType(); }
	static EEditorPlusMenuType StaticType() { return EEditorPlusMenuType::MenuTool; }

protected:
	TSharedRef<IEditorPlusToolInterface> Tool;
};



