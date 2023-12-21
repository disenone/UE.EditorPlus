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

auto CreateClickLambda(const FString& Msg)
{
	return FExecuteAction::CreateLambda([Msg]
		{
			UE_LOG(LogMenuTest, Display, TEXT("Click [%s]"), ToCStr(Msg));
		});
}

void FMenuTest::RegisterPath(const FString& Path, const bool ShouldSuccess)
{
	const TSharedPtr<FEditorPlusMenuBase> Ret = FEditorPlusPath::RegisterPathAction(Path, CreateClickLambda(Path));
	checkf(
		ShouldSuccess == Ret.IsValid(),
		TEXT("RegisterPath [%s] should be [%s], got [%s]"),
		ToCStr(Path),
		ShouldSuccess? "Success" : "Failed",
		Ret.IsValid() ? "Success" : "Failed");
	Menus.Push(Ret);	
}


void FMenuTest::BuildPathMenu()
{
	RegisterPath("/MenuTest/SubMenu1/SubMenu1/Path1");
	RegisterPath("/MenuTest/SubMenu1/SubMenu1/Path2");
	RegisterPath("/<Hook>Help/<MenuBar>MenuTest/<SubMenu>SubMenu1/<Section>Section1/Path3");
	RegisterPath("MenuTest/<SubMenu>SubMenu1/<Section>Section1/Path4");
	RegisterPath("/<Hook>Help/<MenuBar>MenuTest/<SubMenu>SubMenu1/<Section>Section1/<Separator>Separator1/Path5");
	RegisterPath("<Hook>Separator1/Path6");
	RegisterPath("<Hook>Section1/Path7");
	RegisterPath("/<Hook>Help/<MenuBar>MenuTest/<SubMenu>SubMenu1/<Section>Section1/<Section>Section2/ShouldFail", false);
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
				->BindAction(CreateClickLambda("Custom Command1")),
				
				NEW_EP_MENU(FEditorPlusCommand)("Command2")
				->BindAction(CreateClickLambda("Custom Command2")),
			}),

			NEW_EP_MENU(FEditorPlusSeparator)("Separator1"),
		
			NEW_EP_MENU(FEditorPlusSubMenu)("SubMenu1")
			->Content({
				NEW_EP_MENU(FEditorPlusCommand)("Command3")
				->BindAction(CreateClickLambda("Custom Command3")),
				
				NEW_EP_MENU(FEditorPlusCommand)("Command4")
				->BindAction(CreateClickLambda("Custom Command4")),
				
				NEW_EP_MENU(FEditorPlusCommand)("Command5")
				->BindAction(
					FMenuTestCommands::Get(),
					CreateClickLambda("Custom Command5")),
				
				NEW_EP_MENU(FEditorPlusWidget)("Widget1")
				->BindWidget(
					SNew(SHorizontalBox)
					 + SHorizontalBox::Slot()
					 .AutoWidth()
					 [
						 SAssignNew(InputText, SEditableTextBox)
						 .MinDesiredWidth(50)
						 .Text(FText::FromName("Widget"))
					 ]
					 + SHorizontalBox::Slot()
					 .AutoWidth()
					 .Padding(5, 0, 0, 0)
					 [
						 SNew(SButton)
						 .Text(FText::FromName("Widget"))
						 .OnClicked(FOnClicked::CreateSP(this, &FMenuTest::OnClickButton))
					 ]
					),
				
				NEW_EP_MENU(FEditorPlusCommand)("Command6")
				->BindAction(
					FMenuTestCommands::Get(),
					CreateClickLambda("Custom Command6")),
			})
	}));

}


void FMenuTest::BuildMixMenu()
{
	Menus.Push(FEditorPlusPath::RegisterPath(
		"MenuTest/<SubMenu>SubMenu1/<Section>MixSection1",
		NEW_EP_MENU(FEditorPlusSection)("CustomSection1", "CustomSection1", "CustomSection1")
		->Content({
			NEW_EP_MENU(FEditorPlusCommand)("MixPath1")
			->BindAction(CreateClickLambda("MixPath1")),
					
			NEW_EP_MENU(FEditorPlusWidget)("Widget1")
			->BindWidget(
				SNew(SHorizontalBox)
				 + SHorizontalBox::Slot()
				 .AutoWidth()
				 [
					 SAssignNew(InputText, SEditableTextBox)
					 .MinDesiredWidth(50)
					 .Text(FText::FromName("MixWidget"))
				 ]
				 + SHorizontalBox::Slot()
				 .AutoWidth()
				 .Padding(5, 0, 0, 0)
				 [
					 SNew(SButton)
					 .Text(FText::FromName("MixWidget"))
					 .OnClicked(FOnClicked::CreateSP(this, &FMenuTest::OnClickButton))
				 ]
				)
		})	
	));

	RegisterPath("<Hook>CustomSection1/MixPath2");
}
