#pragma once

class EDITORPLUS_API FEditorPlusCommandInfo
{
public:
	explicit FEditorPlusCommandInfo(
		const FExecuteAction& InExecuteAction,
		const EUserInterfaceActionType& InType = EUserInterfaceActionType::Button,
		const FSlateIcon& InIcon = FSlateIcon())
			: Type(InType), Icon(InIcon), ExecuteAction(InExecuteAction) {}

	explicit FEditorPlusCommandInfo(
		const TSharedRef<FUICommandInfo>& InCommandInfo,
		const TSharedRef<FUICommandList>& InCommandList)
			: CommandInfo(InCommandInfo), CommandList(InCommandList) {}

	const EUserInterfaceActionType Type = EUserInterfaceActionType::None;
	const FSlateIcon Icon;
	const FExecuteAction ExecuteAction;
	TSharedPtr<FUICommandInfo> CommandInfo;
	TSharedPtr<FUICommandList> CommandList;
};
