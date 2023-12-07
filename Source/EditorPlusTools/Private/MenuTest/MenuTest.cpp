#include "MenuTest.h"
#include "EditorPlusUtils.h"

DEFINE_LOG_CATEGORY(LogMenuTest);

class FMenuTestCommands: public TEditorPlusCommands<FMenuTestCommands>
{
public:
	FMenuTestCommands()
		: TEditorPlusCommands<FMenuTestCommands>(
			TEXT("MenuTest"),
			TEXT("MenuTest"),
			NAME_None,
			FAppStyle::GetAppStyleSetName())
	{}
};

void FMenuTest::OnStartup()
{
	BuildMenu();
	// BuildSubMenu();
	// RegisterConsoleCommand();
	BuildPathMenu();
}

void FMenuTest::OnShutdown()
{
}

void FMenuTest::BuildMenu()
{
	if (!Menu.IsValid())
	{
		Menu = 
    	NEW_ED_MENU(FEditorPlusMenuBar)("MenuTest", "Open the MenuTest Menu", "MenuTest")
    	->AddMenuBarExtension(TEXT("EditorPlusTools"), EExtensionHook::After)
		;
	}

	Menu->AddChildren({
	NEW_ED_MENU(FEditorPlusSection)("Section 1")
	->Content({
		NEW_ED_MENU(FEditorPlusMenu)(
			"Command1",
			"Command1 tips",
			FExecuteAction::CreateLambda([]
			{
				UE_LOG(LogMenuTest, Display, TEXT("clicked Command1"));
			})),
		NEW_ED_MENU(FEditorPlusMenu)(
			"Command2",
			"Command2 tips",
			FExecuteAction::CreateLambda([]
			{
				UE_LOG(LogMenuTest, Display, TEXT("clicked Command2"));
			})),
	}),
	NEW_ED_MENU(FEditorPlusSeparator)(),
	NEW_ED_MENU(FEditorPlusSubMenu)("Sub Menu 1")
	->Content({
		NEW_ED_MENU(FEditorPlusMenu)(
			"Command3",
			"Command3 tips",
			FExecuteAction::CreateLambda([]
			{
				UE_LOG(LogMenuTest, Display, TEXT("clicked Command3"));
			})),
		NEW_ED_MENU(FEditorPlusMenu)(
			"Command4",
			"Command4 tips",
			FExecuteAction::CreateLambda([]
			{
				UE_LOG(LogMenuTest, Display, TEXT("clicked Command4"));
			})),
		NEW_ED_MENU(FEditorPlusMenu)(
			FMenuTestCommands::Get(),
			"Command5",
			"Command5",
			"Command1 tips",
			FExecuteAction::CreateLambda([]
			{
				UE_LOG(LogMenuTest, Display, TEXT("clicked Command5"));
			})),
		NEW_ED_MENU(FEditorPlusWidget)(
			SNew(SHorizontalBox)
			 + SHorizontalBox::Slot()
			 .AutoWidth()
			 [
				 SAssignNew(InputText, SEditableTextBox)
				 .MinDesiredWidth(50)
				 .Text(FText::FromName("FMenuTest"))
			 ]
			 + SHorizontalBox::Slot()
			 .AutoWidth()
			 .Padding(5, 0, 0, 0)
			 [
				 SNew(SButton)
				 .Text(FText::FromName("FMenuTest"))
				 .OnClicked(FOnClicked::CreateSP(this, &FMenuTest::OnClickButton))
			 ]
			),
		NEW_ED_MENU(FEditorPlusMenu)(
			FMenuTestCommands::Get(),
			"Command6",
			"Command6",
			"Command6 tips",
			FExecuteAction::CreateLambda([]
			{
				UE_LOG(LogMenuTest, Display, TEXT("clicked Command6"));
			})),
	})
	});

}

void FMenuTest::BuildSubMenu()
{
	if (!SubMenu.IsValid())
	{
		SubMenu = 
		NEW_ED_MENU(FEditorPlusSubMenu)("MenuTestSub", "Open the MenuTest Menu")
		->AddMenuExtension(TEXT("Miscellaneous"), EExtensionHook::After, "MenuTestSub")
		->Content
		({
			NEW_ED_MENU(FEditorPlusMenu)(
				FMenuTestCommands::Get(),
				"Command7",
				"Command7",
				"Command7 tips",
				FExecuteAction::CreateLambda([]
				{
					UE_LOG(LogMenuTest, Display, TEXT("clicked Command7"));
				})),	
		})
		;
	}	
}


void FMenuTest::RegisterConsoleCommand()
{
	IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("EditorPlusTools.Test"),
		TEXT("EditorPlusTools.Test"),
		FConsoleCommandDelegate::CreateSP(this, &FMenuTest::BuildSubMenu));	
}

FReply FMenuTest::OnClickButton()
{
	FString Text = InputText->GetText().ToString();

	TArray<FString> Splited = FEditorPlusUtils::SplitString(Text, "/");
	FString OutSplit = FString::Join(Splited, TEXT(","));
	
	UE_LOG(LogMenuTest, Display, TEXT("Split FString %s -> [%s]"), ToCStr(Text), ToCStr(OutSplit));
	
	return FReply::Handled();
}

void FMenuTest::BuildPathMenu()
{
	if (!PathMenus.Num())
	{
		PathMenus.Add(MakeShared<FEditorPlusPathMenu>("/MenuTest/Sub Menu 1/Sub Menu 1/a", FExecuteAction::CreateLambda([]
		{
			UE_LOG(LogMenuTest, Display, TEXT("/MenuTest/Sub Menu 1/Sub Menu 1/a"));
		})));

		PathMenus.Add(MakeShared<FEditorPlusPathMenu>("/MenuTest/Sub Menu 1/Sub Menu 1/a", FExecuteAction::CreateLambda([]
		{
			UE_LOG(LogMenuTest, Display, TEXT("/MenuTest/Sub Menu 1/Sub Menu 1/a"));
		}), true));

		PathMenus.Add(MakeShared<FEditorPlusPathMenu>("/MenuTest/Sub Menu 1/Sub Menu 1/b", FExecuteAction::CreateLambda([]
		{
			UE_LOG(LogMenuTest, Display, TEXT("/MenuTest/Sub Menu 1/Sub Menu 1/b"));
		}), true));
		PathMenus.Add(MakeShared<FEditorPlusPathMenu>("/MenuTest/Sub Menu 1/Sub Menu 1/b", FExecuteAction::CreateLambda([]
		{
			UE_LOG(LogMenuTest, Display, TEXT("/MenuTest/Sub Menu 1/Sub Menu 1/b"));
		})));	
	}
}
