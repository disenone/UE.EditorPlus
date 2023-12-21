#include "MenuTest.h"
#include "EditorPlusPath.h"
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
	BuildPathMenu();
	BuildCustomMenu();
	BuildMixMenu();
}

void FMenuTest::OnShutdown()
{
	for(auto Menu: Menus)
	{
		if(Menu.IsValid()) Menu->Destroy();
	}
	Menus.Empty();
}


void FMenuTest::RegisterPathSuccess(const FString& Path)
{
	const TSharedPtr<FEditorPlusMenuBase> Ret =
		FEditorPlusPath::RegisterPathAction(Path, FExecuteAction::CreateLambda([Path]
		{
			UE_LOG(LogMenuTest, Display, TEXT("Click [%s]"), ToCStr(Path));
		}));
	checkf(Ret.IsValid(), TEXT("failed to register [%s]"), ToCStr(Path))
	Menus.Push(Ret);	
}


void FMenuTest::BuildPathMenu()
{
	RegisterPathSuccess("/MenuTest/SubMenu1/SubMenu1/Path1");
	RegisterPathSuccess("/MenuTest/SubMenu1/SubMenu1/Path2");
	RegisterPathSuccess("/<Hook>Help/<MenuBar>MenuTest/<SubMenu>SubMenu1/<Section>Section1/Path3");
	RegisterPathSuccess("MenuTest/<SubMenu>SubMenu1/<Section>Section1/Path4");
	RegisterPathSuccess("/<Hook>Help/<MenuBar>MenuTest/<SubMenu>SubMenu1/<Section>Section1/<Separator>Separator1/Path5");
	RegisterPathSuccess("<Hook>Separator1/Path6");
	RegisterPathSuccess("<Hook>Section1/Path7");
}


FReply FMenuTest::OnClickButton() const
{
	const FString Text = InputText->GetText().ToString();

	const TArray<FString> Split = FEditorPlusUtils::SplitString(Text, "/");
	const FString OutSplit = FString::Join(Split, TEXT(","));
	
	UE_LOG(LogMenuTest, Display, TEXT("Split FString %s -> [%s]"), ToCStr(Text), ToCStr(OutSplit));
	
	return FReply::Handled();
}

void FMenuTest::BuildCustomMenu()
{
	Menus.Push( 
		NEW_EP_MENU(FEditorPlusMenuBar)("MenuTestCustom")
		->RegisterPath()
		->Content({
			NEW_EP_MENU(FEditorPlusSection)("CustomSection1")
			->Content({
				NEW_EP_MENU(FEditorPlusCommand)("Command1")
				->BindAction(
					FExecuteAction::CreateLambda([]
					{
						UE_LOG(LogMenuTest, Display, TEXT("clicked Command1"));
					})),
				
				NEW_EP_MENU(FEditorPlusCommand)("Command2")
				->BindAction(
					FExecuteAction::CreateLambda([]
					{
						UE_LOG(LogMenuTest, Display, TEXT("clicked Command2"));
					})),
			}),

			NEW_EP_MENU(FEditorPlusSeparator)("Separator1"),
		
			NEW_EP_MENU(FEditorPlusSubMenu)("SubMenu1")
			->Content({
				NEW_EP_MENU(FEditorPlusCommand)("Command3")
				->BindAction(
					FExecuteAction::CreateLambda([]
					{
						UE_LOG(LogMenuTest, Display, TEXT("clicked Command3"));
					})),
				
				NEW_EP_MENU(FEditorPlusCommand)("Command4")
				->BindAction(
					FExecuteAction::CreateLambda([]
					{
						UE_LOG(LogMenuTest, Display, TEXT("clicked Command4"));
					})),
				
				NEW_EP_MENU(FEditorPlusCommand)("Command5")
				->BindAction(
					FMenuTestCommands::Get(),
					FExecuteAction::CreateLambda([]
					{
						UE_LOG(LogMenuTest, Display, TEXT("clicked Command5"));
					})),
				
				NEW_EP_MENU(FEditorPlusWidget)("Widget1")
				->BindWidget(
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
				
				NEW_EP_MENU(FEditorPlusCommand)("Command6")
				->BindAction(
					FMenuTestCommands::Get(),
					FExecuteAction::CreateLambda([]
					{
						UE_LOG(LogMenuTest, Display, TEXT("clicked Command6"));
					})),
			})
	}));

}



void FMenuTest::BuildMixMenu()
{
	// if (!SubMenu.IsValid())
	// {
	// 	SubMenu = 
	// 	NEW_ED_MENU(FEditorPlusSubMenu)("MenuTestSub", "Open the MenuTest Menu")
	// 	->AddMenuExtension(TEXT("Miscellaneous"), EExtensionHook::After, "MenuTestSub")
	// 	->Content
	// 	({
	// 		NEW_ED_MENU(FEditorPlusMenu)(
	// 			FMenuTestCommands::Get(),
	// 			"Command7",
	// 			"Command7",
	// 			"Command7 tips",
	// 			FExecuteAction::CreateLambda([]
	// 			{
	// 				UE_LOG(LogMenuTest, Display, TEXT("clicked Command7"));
	// 			})),	
	// 	})
	// 	;
	// }	
}
