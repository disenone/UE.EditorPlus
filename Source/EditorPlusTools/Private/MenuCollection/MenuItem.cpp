#include "MenuItem.h"


struct RecursiveCommandDataGetter final
{
	static TArray<TSharedPtr<IMenuItem>> CollectMenuItems()
	{
		TArray<TSharedPtr<IMenuItem>> Ret;
		for (auto& Pair : FEditorPlusUtils::GetLevelEditorModule().GetGlobalLevelEditorActions()->UICommandBindingMap)
		{
			auto& Key = Pair.Key;
			auto& Value = Pair.Value;
			Ret.Push(MakeShared<FMenuItemCommand>(Key, Value));
		}

		for (auto& TabEntry: FGlobalTabmanager::Get()->CollectSpawners())
		{
			if(TabEntry.IsValid()) Ret.Push(MakeShared<FMenuItemTab>(TabEntry.Pin()));
		}

		Ret.Sort([](const TSharedPtr<IMenuItem>& Left, const TSharedPtr<IMenuItem>& Right)
		{
			return Left->GetName().Compare(Right->GetName()) < 0;
		});

		return Ret;
	}
};

TArray<TSharedPtr<IMenuItem>> IMenuItem::CollectMenuItems()
{
	return RecursiveCommandDataGetter::CollectMenuItems();
}
