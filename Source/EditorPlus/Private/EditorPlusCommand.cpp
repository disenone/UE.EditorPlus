#include "EditorPlusCommand.h"


void FEditorPlusCommandInfo::Register(FBindingContext* Context)
{
	static const FString UICommandsStr(TEXT("UICommands"));
	const FString Namespace = !LoctextNamespace.IsNone() ? UICommandsStr + TEXT(".") + Namespace : UICommandsStr;
	
	const FString NameString = Name.ToString();
	FUICommandInfo::MakeCommandInfo(
		Context->AsShared(),
		Info, Name,
		FInternationalization::ForUseOnlyByLocMacroAndGraphNodeTextLiterals_CreateText(ToCStr(FriendlyName.ToString()), *Namespace, ToCStr(Name.ToString())),
		FInternationalization::ForUseOnlyByLocMacroAndGraphNodeTextLiterals_CreateText(ToCStr(Desc.ToString()), *Namespace, ToCStr(NameString + TEXT("_ToolTip"))),
		Icon, Type, Chord
	);
}

void FEditorPlusCommandInfo::Unregister(FBindingContext* Context)
{
	if (Info.IsValid())
	{
		FUICommandInfo::UnregisterCommandInfo(Context->AsShared(), Info.ToSharedRef());
	}
}


void FEditorPlusCommandInfo::MapAction(TSharedRef<FUICommandList> CommandList)
{
	CommandList->MapAction(Info, ExecuteAction, FCanExecuteAction());
}

void FEditorPlusCommandInfo::UnmapAction(TSharedRef<FUICommandList> CommandList)
{
	CommandList->UnmapAction(Info);
}
