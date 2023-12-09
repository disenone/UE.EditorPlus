#pragma once

#include "EditorPlusMenu.h"

class FEditorPlusPathSubMenu;
class FEditorPlusPathCommand;
class FEditorPlusPathMenuBar;


class FEditorPlusPathSubMenu: public TEditorPlusMenuBase<FEditorPlusPathSubMenu>
{
public:
	explicit FEditorPlusPathSubMenu(const FName& Name=""): Name(Name) {}
	
	virtual void Register(FMenuBuilder& MenuBuilder, const FName& ParentPath) override;
	
	virtual FName GetName() override { return Name; }
	
	static FName TypeName() { return "FEditorPlusPathSubMenu"; }
	
	virtual FName GetType() const override { return TypeName(); }
	
	virtual void RegisterWithMerge(FMenuBuilder& MenuBuilder, const bool bMerge) override;
protected:
	void MakeSubMenu(FMenuBuilder& MenuBuilder);
	void MakeSubMenu(FMenuBuilder& MenuBuilder, const bool bMerge);
	const FName Name;
};


class FEditorPlusPathCommand: public TEditorPlusMenuBase<FEditorPlusPathCommand>
{
public:
	explicit FEditorPlusPathCommand(const FName& Name, const FExecuteAction& ExecuteAction, const bool bMergeSubMenu=false);
	
	virtual ~FEditorPlusPathCommand() override;
	
	virtual void Register(FMenuBuilder& MenuBuilder, const FName& ParentPath) override;
	
	virtual void Unregister() override;
	
	virtual FName GetName() override { return CommandInfo->Name; }
	
	static FName TypeName() { return "FEditorPlusPathMenu"; }
	
	virtual FName GetType() const override { return TypeName(); }

	virtual bool HasMergeMenu(const bool bMerge) const override { return bMergeSubMenu == bMerge; }
protected:
	TSharedPtr<FEditorPlusCommandInfo> CommandInfo;
	bool bMergeSubMenu;
};


class FEditorPlusPathMenuBar: public TEditorPlusMenuBase<FEditorPlusPathMenuBar>
{
public:
	explicit FEditorPlusPathMenuBar(const FName& Name): Name(Name){}
	
	virtual ~FEditorPlusPathMenuBar() override;
	
	virtual void Register(FMenuBarBuilder& MenuBarBuilder) override;
	
	virtual void Unregister() override;

	using TEditorPlusMenuBase<FEditorPlusPathMenuBar>::AddMenuBarExtension;
	
	using TEditorPlusMenuBase<FEditorPlusPathMenuBar>::RemoveMenuBarExtension;
	
	virtual FName GetName() override { return Name; }
	
	static FName TypeName() { return "FEditorPlusPathMenuBar"; }
	
	virtual FName GetType() const override { return TypeName(); }

	virtual void RegisterWithMerge(FMenuBuilder& MenuBuilder, const bool bMerge) override;
protected:
	virtual void DoAddMenuBarExtension(const FName& ExtensionHook, EExtensionHook::Position Position, const FName& UniqueId) override;
	virtual void OnMenuBarExtension(FMenuBarBuilder& MenuBarBuilder) override;
	virtual void RemoveMenuBarExtension() override;
	virtual void OnMenuModuleChanged(FName ModuleThatChanged, EModuleChangeReason ReasonForChange) override;
	virtual void OnMenuExtension(FMenuBuilder& MenuBuilder) override;
	
	const FName Name;
};
