#include "EditorPlusCommandInfo.h"


void FEditorPlusCommandInfo::Register(FBindingContext* Context)
{
	FUICommandInfo::MakeCommandInfo(
		Context->AsShared(),
		Info, Name,
		Label,
		Tips,
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
