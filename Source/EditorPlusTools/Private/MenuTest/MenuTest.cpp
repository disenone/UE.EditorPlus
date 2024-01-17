#include "MenuTest.h"

#include "EditorPlusPath.h"
#include "EditorPlusUtils.h"

DEFINE_LOG_CATEGORY(LogMenuTest);

#define LOCTEXT_NAMESPACE "EditorPlusTools"

class FMenuTestCommands : public TCommands<FMenuTestCommands>
{
public:
	FMenuTestCommands()
	: TCommands
		(
			"MenuTest", // Context name for fast lookup
			LOCTEXT("FMenuTestCommands", "FMenuTestCommands"), // Localized context name for displaying
			"LevelEditor", // Parent
			FAppStyle::GetAppStyleSetName() // Icon Style Set
		)
	{}

	virtual void RegisterCommands() override
	{
		UI_COMMAND(Command6, "Command6", "Command6 Tips", EUserInterfaceActionType::Button, FInputChord());
	}

	TSharedPtr<FUICommandInfo> Command6;

};


void FMenuTest::OnStartup()
{
	BuildPathMenu();
	BuildCustomMenu();
	BuildMixMenu();
	BuildExtendMenu();
	// BuildTestMenu();
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

TSharedPtr<FEditorPlusMenuBase> FMenuTest::RegisterPath(const FString& Path, const bool ShouldSuccess, const FText& FriendlyName, const FText& FriendlyTips)
{
	TSharedPtr<FEditorPlusMenuBase> Ret = FEditorPlusPath::RegisterPathAction(
		Path, CreateClickLambda(Path), EP_FNAME_HOOK_AUTO, FriendlyName, FriendlyTips);
	checkf(
		ShouldSuccess == Ret.IsValid(),
		TEXT("RegisterPath [%s] should be [%s], got [%s]"),
		ToCStr(Path),
		ShouldSuccess? "Success" : "Failed",
		Ret.IsValid() ? "Success" : "Failed");
	if(Ret.IsValid()) Menus.Push(Ret);
	return Ret;
}


void FMenuTest::BuildPathMenu()
{
	RegisterPath("/MenuTest/SubMenu1/SubMenu1/Path1", true, LOCTEXT("Path1", "Path1"), LOCTEXT("Path1Tips", "Path1Tips"));
	RegisterPath("/MenuTest/SubMenu1/SubMenu1/Path2", true, LOCTEXT("Path2", "Path2"), LOCTEXT("Path2Tips", "Path2Tips"));
	RegisterPath("/<Hook>Help/<MenuBar>MenuTest/<SubMenu>SubMenu1/<Section>Section1/Path3", true, LOCTEXT("Path3", "Path3"), LOCTEXT("Path3Tips", "Path3Tips"));
	RegisterPath("MenuTest/<SubMenu>SubMenu1/<Section>Section1/Path4", true, LOCTEXT("Path4", "Path4"), LOCTEXT("Path4Tips", "Path4Tips"));
	RegisterPath("/<Hook>Help/<MenuBar>MenuTest/<SubMenu>SubMenu1/<Section>Section1/<Separator>Separator1/Path5", true, LOCTEXT("Path5", "Path5"), LOCTEXT("Path5Tips", "Path5Tips"));
	RegisterPath("<Hook>Separator1/Path6", true, LOCTEXT("Path6", "Path6"), LOCTEXT("Path6Tips", "Path6Tips"));
	RegisterPath("<Hook>Section1/Path7", true, LOCTEXT("Path7", "Path7"), LOCTEXT("Path7Tips", "Path7Tips"));
	RegisterPath("/<Hook>Help/<MenuBar>MenuTest/<SubMenu>SubMenu1/<Section>Section1/<Section>Section2/ShouldFail", false, LOCTEXT("ShouldFail", "ShouldFail"), LOCTEXT("ShouldFailTips", "ShouldFailTips"));

	// register path node loctext
	FEditorPlusPath::GetNodeByPath("/MenuTest")->SetFriendlyName(LOCTEXT("MenuTest", "MenuTest"))->SetFriendlyTips(LOCTEXT("MenuTestTips", "MenuTestTips"));
	FEditorPlusPath::GetNodeByPath("/MenuTest/<SubMenu>SubMenu1")->SetFriendlyName(LOCTEXT("SubMenu1", "SubMenu1"))->SetFriendlyTips(LOCTEXT("SubMenu1Tips", "SubMenu1Tips"));
	FEditorPlusPath::GetNodeByPath("/MenuTest/<SubMenu>SubMenu1/<SubMenu>SubMenu1")->SetFriendlyName(LOCTEXT("SubMenu1", "SubMenu1"))->SetFriendlyTips(LOCTEXT("SubMenu1Tips", "SubMenu1Tips"));
	FEditorPlusPath::GetNodeByPath("/<Hook>Help/<MenuBar>MenuTest/<SubMenu>SubMenu1/<Section>Section1")->SetFriendlyName(LOCTEXT("Section1", "Section1"))->SetFriendlyTips(LOCTEXT("Section1Tips", "Section1Tips"));

	const auto Section = FEditorPlusPath::GetNodeByPath("/<Hook>Help/<MenuBar>MenuTest/<SubMenu>SubMenu1/<Section>Section1");
	checkf(
		FEditorPlusPath::RegisterChildPathAction(
			Section.ToSharedRef(), "Path8", CreateClickLambda("Path8"), EP_FNAME_HOOK_AUTO,
			LOCTEXT("Path8", "Path8"), LOCTEXT("Path8Tips", "Path8Tips"))
		.IsValid(),
		TEXT("RegisterChildPath Path8 failed"));
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
	FMenuTestCommands::Register();
	auto Commands = FMenuTestCommands::Get();
	CommandList = MakeShared<FUICommandList>();
	CommandList->MapAction(Commands.Command6, CreateClickLambda("Custom Command6"));

	Menus.Push( 
		EP_NEW_MENU(FEditorPlusMenuBar)("MenuTestCustom", NAME_None, LOCTEXT("MenuTestCustom", "MenuTestCustom"), LOCTEXT("MenuTestCustomTips", "MenuTestCustomTips"))
		->RegisterPath()
		->Content({
			EP_NEW_MENU(FEditorPlusSection)("CustomSection1", NAME_None, LOCTEXT("CustomSection1", "CustomSection1"))
			->Content({
				EP_NEW_MENU(FEditorPlusCommand)("Command1", NAME_None, LOCTEXT("Command1", "Command1"), LOCTEXT("Command1Tips", "Command1Tips"))
				->BindAction(CreateClickLambda("Custom Command1")),
				
				EP_NEW_MENU(FEditorPlusCommand)("Command2", NAME_None, LOCTEXT("Command2", "Command2"), LOCTEXT("Command2Tips", "Command2Tips"))
				->BindAction(CreateClickLambda("Custom Command2")),
			}),

			EP_NEW_MENU(FEditorPlusSeparator)("Separator1"),
		
			EP_NEW_MENU(FEditorPlusSubMenu)("SubMenu1", NAME_None, LOCTEXT("SubMenu1", "SubMenu1"), LOCTEXT("SubMenu1Tips", "SubMenu1Tips"))
			->Content({
				EP_NEW_MENU(FEditorPlusCommand)("Command3", NAME_None, LOCTEXT("Command3", "Command3"), LOCTEXT("Command3Tips", "Command3Tips"))
				->BindAction(CreateClickLambda("Custom Command3")),
				
				EP_NEW_MENU(FEditorPlusCommand)("Command4", NAME_None, LOCTEXT("Command4", "Command4"), LOCTEXT("Command4Tips", "Command4Tips"))
				->BindAction(CreateClickLambda("Custom Command4")),
				
				EP_NEW_MENU(FEditorPlusWidget)("CustomWidget", LOCTEXT("CustomWidget", "CustomWidget"))
				->BindWidget(
					SNew(SHorizontalBox)
					 + SHorizontalBox::Slot()
					 .AutoWidth()
					 [
						 SAssignNew(InputText, SEditableTextBox)
						 .MinDesiredWidth(50)
						 .Text(FText::FromName("CustomWidget"))
					 ]
					 + SHorizontalBox::Slot()
					 .AutoWidth()
					 .Padding(5, 0, 0, 0)
					 [
						 SNew(SButton)
						 .Text(FText::FromName("CustomWidget"))
						 .OnClicked(FOnClicked::CreateSP(this, &FMenuTest::OnClickButton))
					 ]
					),
				
				EP_NEW_MENU(FEditorPlusCommand)("Command6", NAME_None, LOCTEXT("Command6", "Command6"), LOCTEXT("Command6Tips", "Command6Tips"))
				->BindCommand(
					Commands.Command6.ToSharedRef(),
					CommandList.ToSharedRef())
			})
	}));

}


void FMenuTest::BuildMixMenu()
{
	Menus.Push(FEditorPlusPath::RegisterPath(
		"MenuTest/<SubMenu>SubMenu1/<Section>MixSection1",
		EP_NEW_MENU(FEditorPlusSection)("MixSection1", "MixSection1", LOCTEXT("MixSection1", "MixSection1"))
		->Content({
			EP_NEW_MENU(FEditorPlusCommand)("MixPath1", "MixPath1", LOCTEXT("MixPath1", "MixPath1"), LOCTEXT("MixPath1Tips", "MixPath1Tips"))
			->BindAction(CreateClickLambda("MixPath1")),
					
			EP_NEW_MENU(FEditorPlusWidget)("MixWidget", LOCTEXT("MixWidget", "MixWidget"))
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

	RegisterPath("<Hook>MixSection1/MixPath2", true, LOCTEXT("MixPath2", "MixPath2"), LOCTEXT("MixPath2Tips", "MixPath2Tips"));
}

void FMenuTest::BuildExtendMenu()
{
	Menus.Push(FEditorPlusPath::RegisterPath(
		"<Hook>EpicGamesHelp/<Separator>ExtendSeparator",
		EP_NEW_MENU(FEditorPlusSeparator)("ExtendSeparator")
		->Content({
			EP_NEW_MENU(FEditorPlusCommand)("ExtendPath1", "ExtendPath1", LOCTEXT("ExtendPath1", "ExtendPath1"), LOCTEXT("ExtendPath1Tips", "ExtendPath1Tips"))
			->BindAction(CreateClickLambda("ExtendPath1")),
						
			EP_NEW_MENU(FEditorPlusWidget)("ExtendWidget", LOCTEXT("ExtendWidget", "ExtendWidget"))
			->BindWidget(
				SNew(SHorizontalBox)
				 + SHorizontalBox::Slot()
				 .AutoWidth()
				 [
					 SAssignNew(InputText, SEditableTextBox)
					 .MinDesiredWidth(50)
					 .Text(FText::FromName("ExtendWidget"))
				 ]
				 + SHorizontalBox::Slot()
				 .AutoWidth()
				 .Padding(5, 0, 0, 0)
				 [
					 SNew(SButton)
					 .Text(FText::FromName("ExtendWidget"))
					 .OnClicked(FOnClicked::CreateSP(this, &FMenuTest::OnClickButton))
				 ]
				)
		})	
	));
	
	RegisterPath("<Hook>ExtendSeparator/ExtendPath2", true, LOCTEXT("ExtendPath2", "ExtendPath2"), LOCTEXT("ExtendPath2Tips", "ExtendPath2Tips"));
}


void FMenuTest::BuildTestMenu()
{
	auto MenuExtender = MakeShared<FExtender>();

	MenuExtender->AddMenuBarExtension(
		"Help", EExtensionHook::After,
		nullptr,
		FMenuBarExtensionDelegate::CreateLambda([](FMenuBarBuilder& MenuBarBuilder)
		{
			MenuBarBuilder.AddPullDownMenu(
				FText::FromName("MenuTest"),
				FText::FromName("MenuTest"),
				FNewMenuDelegate::CreateLambda([](FMenuBuilder& MenuBuilder)
				{
					MenuBuilder.BeginSection(NAME_None, FText::FromName("MenuTestSection"));
					MenuBuilder.AddMenuSeparator();
					MenuBuilder.AddMenuEntry(
						FText::FromName("MenuTestAction"), FText::FromName("MenuTestAction"),
						FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([]()
						{
							UE_LOG(LogMenuTest, Display, TEXT("MenuTestAction"));
						})));

					MenuBuilder.AddSubMenu(
						FText::FromName("MenuTestSubb"),
						FText::FromName("MenuTestSubb"),
						FNewMenuDelegate::CreateLambda([](FMenuBuilder& MenuBuilder)
						{
							MenuBuilder.AddMenuEntry(
								FText::FromName("MenuTestSubAction"), FText::FromName("MenuTestSubAction"),
								FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([]()
								{
									UE_LOG(LogMenuTest, Display, TEXT("MenuTestSubAction"));
								})));
						}));
					MenuBuilder.EndSection();

					MenuBuilder.AddWidget(
					SNew(SHorizontalBox)
						 + SHorizontalBox::Slot()
						 .AutoWidth()
						 [
							 SNew(SEditableTextBox)
							 .MinDesiredWidth(50)
							 .Text(FText::FromName("MenuTestWidget"))
						 ]
						 + SHorizontalBox::Slot()
						 .AutoWidth()
						 .Padding(5, 0, 0, 0)
						 [
							SNew(SButton)
							.Text(FText::FromName("ExtendWidget"))
							.OnClicked(FOnClicked::CreateLambda([]()
							{
								// do action
								return FReply::Handled();
							}))
						 ],
					 FText::GetEmpty()
					);
				}),
				"MenuTest");
		})
	);

	MenuExtender->AddMenuExtension(
		"Programming", EExtensionHook::After,
		nullptr,
		FMenuExtensionDelegate::CreateLambda([](FMenuBuilder& MenuBuilder)
		{
			MenuBuilder.AddMenuEntry(
			FText::FromName("MenuTestAction"), FText::FromName("MenuTestAction"),
			FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([]()
			{
				UE_LOG(LogMenuTest, Display, TEXT("MenuTestAction"));
			})));
		})
	);

	FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor").GetMenuExtensibilityManager()->AddExtender(MenuExtender);
}

#undef LOCTEXT_NAMESPACE
