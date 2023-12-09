#pragma once

#include "EditorPlusToolInterface.h"
#include "EditorPlusCommand.h"

#define NEW_ED_MENU(Class) MakeShared<Class>


class EDITORPLUS_API FEditorPlusMenuBase: public TSharedFromThis<FEditorPlusMenuBase>
{
public:
	explicit FEditorPlusMenuBase() {}
	
	virtual ~FEditorPlusMenuBase();
	
	virtual void Register(FMenuBuilder& MenuBuilder, const FName& ParentPath=NAME_None);
	
	virtual void Register(FMenuBarBuilder& MenuBuilder) {}

	virtual void RegisterWithMerge(FMenuBuilder& MenuBuilder, const bool bMerge) { Register(MenuBuilder); }
	
	virtual void Unregister();

	virtual TArray<TSharedRef<FEditorPlusMenuBase>>::SizeType ChildrenNum() const { return Children.Num(); }
	
	virtual bool IsEmpty() const { return Children.Num() == 0; }
	
	virtual void AddChild(const TSharedRef<FEditorPlusMenuBase>& Child) { Children.Emplace(Child); }

	virtual bool RemoveChild(const TSharedRef<FEditorPlusMenuBase>& Child) { return Children.Remove(Child) > 0; }
	
	virtual void AddChildren(const TArray<TSharedRef<FEditorPlusMenuBase>>& Menus) { Children.Append(Menus); }
	
	virtual void ClearChildren() { Children.Empty(); }

	TArray<TSharedRef<FEditorPlusMenuBase>> GetChildrenByName(const FName& Name, const FName& TypeName = NAME_None);
	
	template <class Derived>
	TArray<TSharedRef<Derived>> GetChildrenByName(const FName& Name)
	{
		TArray<TSharedRef<FEditorPlusMenuBase>> BaseRet = GetChildrenByName(Name, Derived::TypeName());
		TArray<TSharedRef<Derived>>	Ret;
		Ret.Reserve(BaseRet.Num());
		for(auto Menu: BaseRet)
		{
			Ret.Push(StaticCastSharedRef<Derived>(Menu));
		}
		return Ret;
	}

	TArray<TSharedRef<FEditorPlusMenuBase>> GetChildrenByType(const FName& TypeName = NAME_None);

	template <class Derived>
	TArray<TSharedRef<Derived>> GetChildrenByType()
	{
		TArray<TSharedRef<FEditorPlusMenuBase>> BaseRet = GetChildrenByType(Derived::TypeName());
		TArray<TSharedRef<Derived>>	Ret;
		Ret.Reserve(BaseRet.Num());
		for(auto Menu: BaseRet)
		{
			Ret.Push(StaticCastSharedRef<Derived>(Menu));
		}
		return Ret;
	}
	
	virtual FName GetName() { return NAME_None; }
	virtual FName GetType() const = 0;
	
	template <class Derived>
	bool IsType() const { return GetType() == Derived::TypeName(); }
	
	bool IsType(const FName& Type) const { return Type == NAME_None || GetType() == Type; }

	virtual void RemoveMenuExtension();
	virtual void OnMenuExtension(FMenuBuilder& MenuBuilder);
	virtual void OnMenuModuleChanged(FName ModuleThatChanged, EModuleChangeReason ReasonForChange);

	virtual void RemoveMenuBarExtension();
	virtual void OnMenuBarExtension(FMenuBarBuilder& MenuBarBuilder);
	virtual void OnMenuBarModuleChanged(FName ModuleThatChanged, EModuleChangeReason ReasonForChange);

	virtual bool HasMergeMenu(const bool bMerge) const;
	
protected:
	virtual void SetUniqueId(const FName& UniId=NAME_None) { UniqueId = UniId == NAME_None ? GetName() : UniId; }
	
	virtual void DoAddMenuExtension(const FName& ExtensionHook, EExtensionHook::Position Position, const FName& UniqueId=NAME_None);
	virtual void DoAddMenuBarExtension(const FName& ExtensionHook, EExtensionHook::Position Position, const FName& UniqueId=NAME_None);

protected:
	TArray<TSharedRef<FEditorPlusMenuBase>> Children;
	
	FName UniqueId;
	TSharedPtr<FExtender> MenuExtender;
	FDelegateHandle ModuleChangedHandler;
};

template <class Derived>
class TEditorPlusMenuBase: public FEditorPlusMenuBase 
{
public:
	virtual TSharedRef<Derived> Content(const TArray<TSharedRef<FEditorPlusMenuBase>>& Menus)
	{
		AddChildren(Menus);
		return AsShared();
	}

	virtual TSharedRef<Derived> ClearContent()
	{
		ClearChildren();
		return AsShared();
	}

	TSharedRef<Derived> AsShared()
	{
		return StaticCastSharedRef<Derived>(FEditorPlusMenuBase::AsShared());
	}

protected:
	virtual TSharedRef<Derived> AddMenuExtension(const FName& ExtensionHook, EExtensionHook::Position Position, const FName& UniId=NAME_None)
	{
		DoAddMenuExtension(ExtensionHook, Position, UniId);
		return AsShared();
	}
	
	virtual TSharedRef<Derived> AddMenuBarExtension(const FName& ExtensionHook, EExtensionHook::Position Position, const FName& UniId=NAME_None)
	{
		DoAddMenuBarExtension(ExtensionHook, Position, UniId);
		return AsShared();
	}
};

template <class Derived>
class TEditorPlusMenuBaseNoSlots: public TEditorPlusMenuBase<Derived>
{
private:
	using TEditorPlusMenuBase<Derived>::Content;
	using TEditorPlusMenuBase<Derived>::ClearContent;
	using TEditorPlusMenuBase<Derived>::AddChildren;
	using TEditorPlusMenuBase<Derived>::ClearChildren;
};

class EDITORPLUS_API FEditorPlusSection: public TEditorPlusMenuBase<FEditorPlusSection>
{
public:
	explicit FEditorPlusSection(const FName& Name="", const FName& Hook=""): Name(Name), Hook(Hook) {}
	virtual void Register(FMenuBuilder& MenuBuilder, const FName& ParentPath) override;
	using TEditorPlusMenuBase<FEditorPlusSection>::AddMenuExtension;
	using TEditorPlusMenuBase<FEditorPlusSection>::RemoveMenuExtension;
	virtual FName GetName() override { return Name; }
	static FName TypeName() { return "FEditorPlusSection"; }
	virtual FName GetType() const override { return TypeName(); }
	
protected:
	const FName Name;
	const FName Hook;
};


class EDITORPLUS_API FEditorPlusSeparator: public TEditorPlusMenuBaseNoSlots<FEditorPlusSeparator>
{
public:
	explicit FEditorPlusSeparator(const FName& Hook=""): Hook(Hook) {}
	virtual void Register(FMenuBuilder& MenuBuilder, const FName& ParentPath) override;
	using TEditorPlusMenuBase<FEditorPlusSeparator>::AddMenuExtension;
	using TEditorPlusMenuBase<FEditorPlusSeparator>::RemoveMenuExtension;
	static FName TypeName() { return "FEditorPlusSeparator"; }
	virtual FName GetType() const override { return TypeName(); }
	
protected:
	const FName Hook;
};


class EDITORPLUS_API FEditorPlusSubMenu: public TEditorPlusMenuBase<FEditorPlusSubMenu>
{
public:
	explicit FEditorPlusSubMenu(const FName& Name, const FName& Tips=NAME_None, const FName& Hook=NAME_None)
		: Name(Name), Tips(Tips), Hook(Hook) {}
	virtual void Register(FMenuBuilder& MenuBuilder, const FName& ParentPath) override;
	using TEditorPlusMenuBase<FEditorPlusSubMenu>::AddMenuExtension;
	using TEditorPlusMenuBase<FEditorPlusSubMenu>::RemoveMenuExtension;
	virtual FName GetName() override { return Name; }	
	static FName TypeName() { return "FEditorPlusSubMenu"; }
	virtual FName GetType() const override { return TypeName(); }
	
protected:
	void MakeSubMenu(FMenuBuilder& MenuBuilder, const FName ParentPath);
	const FName Name;
	const FName Tips;
	const FName Hook;
};


class EDITORPLUS_API FEditorPlusMenu: public TEditorPlusMenuBaseNoSlots<FEditorPlusMenu>
{
public:
	explicit FEditorPlusMenu(
		const TSharedRef<IEditorPlusCommandsInterface>& Commands,
		const FName& UniqueName,
		const FName& FriendlyName,
		const FName& Desc,
		const FExecuteAction& ExecuteAction,
		const FName& Hook = NAME_None,
		const EUserInterfaceActionType& Type = EUserInterfaceActionType::Button,
		const FInputChord& Chord = FInputChord(),
		const FName& LoctextNamespace = NAME_None,
		const FSlateIcon& Icon = FSlateIcon());
	
	explicit FEditorPlusMenu(
		const FName& Name,
		const FName& Desc,
		const FExecuteAction& ExecuteAction,
		const FName& Hook = NAME_None,
		const EUserInterfaceActionType& Type = EUserInterfaceActionType::Button,
		const FSlateIcon& InIcon = FSlateIcon());
	
	virtual ~FEditorPlusMenu() override;
	
	virtual void Register(FMenuBuilder& MenuBuilder, const FName& ParentPath) override;
	virtual void Unregister() override;
	
	using TEditorPlusMenuBase<FEditorPlusMenu>::AddMenuExtension;
	using TEditorPlusMenuBase<FEditorPlusMenu>::RemoveMenuExtension;
	virtual FName GetName() override { return CommandInfo->Name; }	
	static FName TypeName() { return "FEditorPlusMenu"; }
	virtual FName GetType() const override { return TypeName(); }	
protected:
	
	TSharedPtr<FEditorPlusCommandInfo> CommandInfo;
	TSharedPtr<IEditorPlusCommandsInterface> CommandMgr;
};


class EDITORPLUS_API FEditorPlusMenuBar: public TEditorPlusMenuBase<FEditorPlusMenuBar>
{
public:
	explicit FEditorPlusMenuBar(const FName& Name, const FName& Desc = NAME_None, const FName& Hook = NAME_None)
		: Name(Name), Desc(Desc), Hook(Hook) {}
	virtual ~FEditorPlusMenuBar() override;
	
	virtual void Register(FMenuBarBuilder& MenuBarBuilder) override;
	virtual void Unregister() override;

	using TEditorPlusMenuBase<FEditorPlusMenuBar>::AddMenuBarExtension;
	using TEditorPlusMenuBase<FEditorPlusMenuBar>::RemoveMenuBarExtension;
	
	virtual FName GetName() override { return Name; }
	static FName TypeName() { return "FEditorPlusMenuBar"; }
	virtual FName GetType() const override { return TypeName(); }	
	
protected:
	virtual void OnMenuExtension(FMenuBuilder& MenuBuilder) override;
	
	const FName Name;
	const FName Desc;
	const FName Hook;
};


class EDITORPLUS_API FEditorPlusWidget: public TEditorPlusMenuBaseNoSlots<FEditorPlusWidget>
{
public:
	explicit FEditorPlusWidget(const TSharedRef<SWidget> Widget): Widget(Widget) {}
	virtual void Register(FMenuBuilder& MenuBuilder, const FName& ParentPath) override;
	
	using TEditorPlusMenuBase<FEditorPlusWidget>::AddMenuExtension;
	using TEditorPlusMenuBase<FEditorPlusWidget>::RemoveMenuExtension;
	static FName TypeName() { return "FEditorPlusWidget"; }
	virtual FName GetType() const override { return TypeName(); }	
	
protected:
	TSharedRef<SWidget> Widget;
};


class EDITORPLUS_API FEditorPlusToolMenu: public TEditorPlusMenuBaseNoSlots<FEditorPlusToolMenu>
{
public:
	explicit FEditorPlusToolMenu(const TSharedRef<IEditorPlusToolInterface> Tool): Tool(Tool) {}
	virtual ~FEditorPlusToolMenu() override { FEditorPlusToolMenu::Unregister(); }
	
	virtual void Register(FMenuBuilder& MenuBuilder, const FName& ParentPath) override;
	virtual void Unregister() override;

	using TEditorPlusMenuBase<FEditorPlusToolMenu>::AddMenuExtension;
	using TEditorPlusMenuBase<FEditorPlusToolMenu>::RemoveMenuExtension;
	static FName TypeName() { return "FEditorPlusToolMenu"; }
	virtual FName GetType() const override { return TypeName(); }	
	
protected:
	TSharedRef<IEditorPlusToolInterface> Tool;
};


class EDITORPLUS_API FEditorPlusPathMenu: public TSharedFromThis<FEditorPlusPathMenu>
{
public:
	explicit FEditorPlusPathMenu(const FName& Path, const FExecuteAction& ExecuteAction, const bool bMergeMenu=true);
	~FEditorPlusPathMenu();

protected:
	const FName Path;
	const FExecuteAction& ExecuteAction;
	TSharedPtr<FEditorPlusMenuBase> Menu;
};
