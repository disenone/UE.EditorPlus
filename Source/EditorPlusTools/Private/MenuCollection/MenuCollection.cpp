#include "MenuCollection.h"
#include "EditorPlusUtils.h"

DEFINE_LOG_CATEGORY(LogMenuCollection);

void FMenuCollection::OnStartup()
{
	BuildMenu();
	// BuildSubMenu();
	// RegisterConsoleCommand();
	// BuildPathMenu();
}

void FMenuCollection::OnShutdown()
{
}

void FMenuCollection::BuildMenu()
{
	if (Menu.IsValid()) return;

	// Menu = 
	// 	NEW_ED_MENU(FEditorPlusSubMenu)("SubMenuTest", "Open the MenuTest Menu", "MenuTest")
	// 	->AddMenuExtension(TEXT("OpenPython"), EExtensionHook::After)
	// 	;
	//
	// Menu->AddChildren({
	// NEW_ED_MENU(FEditorPlusSection)("Section 1", "Section 1")
	// ->Content({
	// 	NEW_ED_MENU(FEditorPlusMenu)(
	// 		"Command1",
	// 		"Command1 tips",
	// 		FExecuteAction::CreateLambda([]
	// 			{
	// 				UE_LOG(LogMenuCollection, Display, TEXT("clicked Command1"));
	// 			}),
	// 		"Command1"),
	// 	NEW_ED_MENU(FEditorPlusMenu)(
	// 		"Command2",
	// 		"Command2 tips",
	// 		FExecuteAction::CreateLambda([]
	// 			{
	// 				UE_LOG(LogMenuCollection, Display, TEXT("clicked Command2"));
	// 			}),
	// 		"Command2"),
	// }),
	// NEW_ED_MENU(FEditorPlusSeparator)("Separator1"),
	// NEW_ED_MENU(FEditorPlusSubMenu)("Sub Menu 1", "Open the Sub Menu 1", "Sub Menu 1")
	// ->Content({
	// 	NEW_ED_MENU(FEditorPlusMenu)(
	// 		"Command3",
	// 		"Command3 tips",
	// 		FExecuteAction::CreateLambda([]
	// 			{
	// 				UE_LOG(LogMenuCollection, Display, TEXT("clicked Command3"));
	// 			}),
	// 		"Command3"),
	// 	NEW_ED_MENU(FEditorPlusMenu)(
	// 		"Command4",
	// 		"Command4 tips",
	// 		FExecuteAction::CreateLambda([]
	// 		{
	// 			UE_LOG(LogMenuCollection, Display, TEXT("clicked Command4"));
	// 		})),
	// })
	// });
}

void FMenuCollection::BuildSubMenu()
{
}



void FMenuCollection::BuildPathMenu()
{
	UToolMenu* ToolMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.File");

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
	
	ToolMenu->AddMenuEntry(TEXT("Test Menu"), Entry);
}
