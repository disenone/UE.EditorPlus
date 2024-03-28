
#include "ResourceInfo.h"
#include "IconBrowser/IconBrowserTab.h"
#include "ColorBrowser/ColorBrowserTab.h"
#include "FontBrowser/FontBrowserTab.h"
#include "SoundBrowser/SoundBrowserTab.h"
#include "ValueBrowser/ValueBrowserTab.h"
#include "WidgetStyleBrowser/WidgetStyleBrowserTab.h"

TSharedRef<TArray<TSharedRef<FResourceInfo>>> FResourceInfo::CollectResources()
{
	static TSharedRef<TArray<TSharedRef<FResourceInfo>>> StaticRet = MakeShared<TArray<TSharedRef<FResourceInfo>>>();

	if(!StaticRet->IsEmpty()) return StaticRet;

#define EP_EMPLACE_RESOURCE_INFO(Name) StaticRet->Emplace(MakeShared<FResourceInfo>(EResourceType::Name, TEXT(#Name)))
	EP_EMPLACE_RESOURCE_INFO(Icon);
	EP_EMPLACE_RESOURCE_INFO(Color);
	EP_EMPLACE_RESOURCE_INFO(Font);
	EP_EMPLACE_RESOURCE_INFO(WidgetStyle);
	EP_EMPLACE_RESOURCE_INFO(Value);
	EP_EMPLACE_RESOURCE_INFO(Sound);
#undef EP_EMPLACE_RESOURCE_INFO

	checkf(StaticRet->Num() == static_cast<uint8>(EResourceType::TypeNum), TEXT("missing resource type"));

	StaticRet->Sort([](const TSharedRef<FResourceInfo>& A, const TSharedRef<FResourceInfo>& B)
	{
		return *A < *B;
	});

	return StaticRet;
}


TSharedRef<SWidget> FResourceInfo::GetTabWidget() const
{
	static TMap<EResourceType, TSharedRef<SWidget>> StaticWidgets;

	if (!StaticWidgets.IsEmpty()) return StaticWidgets[Type];

#define EP_EMPLACE_WIDGET(Name) StaticWidgets.Emplace(EResourceType::Name, SNew(S##Name##BrowserTab));
	EP_EMPLACE_WIDGET(Icon);
	EP_EMPLACE_WIDGET(Color);
	EP_EMPLACE_WIDGET(Font);
	EP_EMPLACE_WIDGET(WidgetStyle);
	EP_EMPLACE_WIDGET(Value);
	EP_EMPLACE_WIDGET(Sound);
#undef EP_EMPLACE_WIDGET

	checkf(StaticWidgets.Num() == static_cast<uint8>(EResourceType::TypeNum), TEXT("missing resource type"));
	return StaticWidgets[Type];
}
