#include "MenuCollection.h"
#include "EditorPlusUtils.h"

DEFINE_LOG_CATEGORY(LogMenuCollection);

void FMenuCollection::OnStartup()
{
	// BuildMenu();
	// BuildSubMenu();
	// RegisterConsoleCommand();
	BuildPathMenu();
}

void FMenuCollection::OnShutdown()
{
}

void FMenuCollection::BuildMenu()
{

}

void FMenuCollection::BuildSubMenu()
{
}



void FMenuCollection::BuildPathMenu()
{
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.File");

	FToolMenuEntry Entry = FToolMenuEntry::InitMenuEntry(
	TEXT("Test Menu"),
	FText::FromString(TEXT("Test Menu")),
	FText::FromString(TEXT("Test Menu")),
	FSlateIcon(),
	FUIAction(
		FExecuteAction::CreateLambda([]
		{
			UE_LOG(LogMenuCollection, Display, TEXT("clicked Test Menu"));
		}),
		FCanExecuteAction()
	)
	);
	Entry.InsertPosition.Name = NAME_None;
	Entry.InsertPosition.Position = EToolMenuInsertType::First;
	
	Menu->AddMenuEntry(TEXT("Test Menu"), Entry);
}
