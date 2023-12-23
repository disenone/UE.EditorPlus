#pragma once

class EDITORPLUS_API FEditorPlusCommandInfo
{
public:
	FEditorPlusCommandInfo(
		const FName& InName,
		const FExecuteAction& InExecuteAction,
		const FText& InLabel = FText::GetEmpty(),
		const FText& InTips = FText::GetEmpty(),
		const FName& InHook = NAME_None,
		const EUserInterfaceActionType& InType = EUserInterfaceActionType::Button,
		const FInputChord& InChord = FInputChord(),
		const FSlateIcon& InIcon = FSlateIcon(),
		const FName& InUniqueId = NAME_None)
		:	Name(InName),
			Label(InLabel.IsEmpty() ? FText::FromName(InName) : InLabel),
			Tips(InTips.IsEmpty() ? FText::FromName(InName) : InTips),
			Hook(InHook),
			Type(InType),
			Chord(InChord),
			Icon(InIcon),
			ExecuteAction(InExecuteAction),
			UniqueId(InUniqueId != NAME_None ? InUniqueId: InName)
	{}

	void Register(FBindingContext* Context);
	void Unregister(FBindingContext* Context);
	void MapAction(TSharedRef<FUICommandList> CommandList);
	void UnmapAction(TSharedRef<FUICommandList> CommandList);
	void SetUniqueId(const FName& InUniqueId) { UniqueId = InUniqueId; }
	
	TSharedPtr<FUICommandInfo> Info;
	const FName Name;
	const FText Label;
	const FText Tips;
	const FName Hook;
	const EUserInterfaceActionType Type;
	const FInputChord Chord;
	const FSlateIcon Icon;
	const FExecuteAction ExecuteAction;
	FName UniqueId;
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
		const FName& UniqueId = CommandInfo->UniqueId;
		if (!CommandInfoMap.Contains(UniqueId))
		{
			CommandContext::RegisterCommand(CommandInfo);
			CommandInfo->MapAction(CommandList);
			CommandInfoMap.Add(UniqueId, CommandInfo);
		}
	}

	virtual void RemoveCommand(TSharedRef<FEditorPlusCommandInfo> CommandInfo) override
	{
		const FName& UniqueId = CommandInfo->UniqueId;
		if (CommandInfoMap.Contains(UniqueId))
		{
			CommandContext::UnregisterCommand(CommandInfo);
			CommandInfo->UnmapAction(CommandList);
			CommandInfoMap.Remove(UniqueId);
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
