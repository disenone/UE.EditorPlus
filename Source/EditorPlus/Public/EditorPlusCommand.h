#pragma once

class EDITORPLUS_API FEditorPlusCommandInfo
{
public:
	FEditorPlusCommandInfo(
		const FName& Name,
		const FExecuteAction& ExecuteAction,
		const FName& FriendlyName = NAME_None,
		const FName& Desc = NAME_None,
		const EUserInterfaceActionType& Type = EUserInterfaceActionType::Button,
		const FInputChord& Chord = FInputChord(),
		const FName& LoctextNamespace = NAME_None,
		const FSlateIcon& Icon = FSlateIcon())
		:	Name(Name), FriendlyName(FriendlyName == NAME_None ? Name: FriendlyName),
			Desc(Desc), Type(Type), Chord(Chord),
			LoctextNamespace(LoctextNamespace), Icon(Icon), ExecuteAction(ExecuteAction) {}

	void Register(FBindingContext* Context);
	void Unregister(FBindingContext* Context);
	void MapAction(TSharedRef<FUICommandList> CommandList);
	void UnmapAction(TSharedRef<FUICommandList> CommandList);
	
	TSharedPtr<FUICommandInfo> Info;
	const FName Name;
	const FName FriendlyName;
	const FName Desc;
	const EUserInterfaceActionType Type;
	const FInputChord Chord;
	const FName LoctextNamespace;
	const FSlateIcon Icon;
	const FExecuteAction ExecuteAction;
};


class IEditorPlusCommandsInterface
{
public:
	virtual ~IEditorPlusCommandsInterface(){}
	virtual void AddCommand(TSharedRef<FEditorPlusCommandInfo> CommandInfo) = 0;
	virtual void RemoveCommand(TSharedRef<FEditorPlusCommandInfo> CommandInfo) = 0;
	
	virtual void Register() = 0;
	virtual void Unregister() = 0;
	virtual TSharedRef<FUICommandList> GetCommandList() = 0;
};

template <class Derived>
class TEditorPlusCommands: public IEditorPlusCommandsInterface
{

public:	
	class CommandContext: public TCommands<CommandContext>
	{
	public:
		CommandContext()
			: TCommands<CommandContext>(
				Derived::Get()->ContextName,
				FText::FromName(Derived::Get()->ContextDesc),
				Derived::Get()->ContextParent,
				Derived::Get()->StyleSetName
				)
		{}

		virtual void RegisterCommands() override
		{
			Derived::Get()->ContextPtr = StaticCastSharedRef<CommandContext>(TCommands<CommandContext>::AsShared());
		}

		static void RegisterCommand(TSharedRef<FEditorPlusCommandInfo> CommandInfo)
		{
			CommandInfo->Register(CommandContext::Instance.Pin().Get());
		}

		static void UnregisterCommand(TSharedRef<FEditorPlusCommandInfo> CommandInfo)
		{
			CommandInfo->Unregister(CommandContext::Instance.Pin().Get());
		}
	};

	virtual ~TEditorPlusCommands() override {}
	static TSharedRef<Derived> Get()
	{
		if (!Instance.IsValid())
		{
			Instance = MakeShareable(new Derived);
			Instance->Register();
		}
		return Instance.ToSharedRef();
	}
	
	virtual void AddCommand(TSharedRef<FEditorPlusCommandInfo> CommandInfo) override
	{
		const FName& UniqueName = CommandInfo->Name;
		if (!CommandInfoMap.Contains(UniqueName))
		{
			CommandContext::RegisterCommand(CommandInfo);
			CommandInfo->MapAction(CommandList);
			CommandInfoMap.Add(UniqueName, CommandInfo);
		}
	}

	virtual void RemoveCommand(TSharedRef<FEditorPlusCommandInfo> CommandInfo) override
	{
		const FName& UniqueName = CommandInfo->Name;
		if (CommandInfoMap.Contains(UniqueName))
		{
			CommandContext::UnregisterCommand(CommandInfo);
			CommandInfo->UnmapAction(CommandList);
			CommandInfoMap.Remove(UniqueName);
		}
	}
	
	virtual void Register() override
	{
		CommandContext::Register();
	}

	virtual void Unregister() override
	{
		CommandContext::Unregister();
	}
	
	virtual TSharedRef<FUICommandList> GetCommandList() override
	{
		return CommandList;	
	}
	
protected:
	TEditorPlusCommands(const FName& InContextName, const FName& InContextDesc, const FName& InContextParent, const FName& InStyleSetName)
		: ContextName(InContextName), ContextDesc(InContextDesc), ContextParent(InContextParent),
		  StyleSetName(InStyleSetName), CommandList(MakeShareable(new FUICommandList)) {}
	
	TEditorPlusCommands(const TEditorPlusCommands&) = delete;
	TEditorPlusCommands& operator=(const TEditorPlusCommands&) = delete;

	static TSharedPtr<Derived> Instance;
	
private:
	const FName ContextName;
	const FName ContextDesc;
	const FName ContextParent;
	const FName StyleSetName;
	TMap<FName, TSharedPtr<FEditorPlusCommandInfo>> CommandInfoMap;
	TSharedRef<FUICommandList> CommandList;
	TSharedPtr<CommandContext> ContextPtr;
};

template <class Derived> TSharedPtr<Derived> TEditorPlusCommands<Derived>::Instance = nullptr;
