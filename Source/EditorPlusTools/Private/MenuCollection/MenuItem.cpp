// Copyright (c) 2024 - 2024, Disenone (https://github.com/disenone).
// All rights reserved. Licensed under MIT License.

#include "MenuItem.h"

class FExposedGlobalTabmanager: public FGlobalTabmanager
{
public:

	TArray< TWeakPtr<FTabSpawnerEntry> > CollectSpawners()
	{
		TArray< TWeakPtr<FTabSpawnerEntry> > AllSpawners;

		// Editor-specific tabs
		for ( FTabSpawner::TIterator SpawnerIterator(TabSpawner); SpawnerIterator; ++SpawnerIterator )
		{
			const TSharedRef<FTabSpawnerEntry>& SpawnerEntry = SpawnerIterator.Value();
			{
				if (IsAllowedTab(SpawnerEntry->GetTabType()))
				{
					AllSpawners.AddUnique(SpawnerEntry);
				}
			}
		}

		// General Tabs
		for ( FTabSpawner::TIterator SpawnerIterator(*NomadTabSpawner); SpawnerIterator; ++SpawnerIterator )
		{
			const TSharedRef<FTabSpawnerEntry>& SpawnerEntry = SpawnerIterator.Value();
			{
				if (IsAllowedTab(SpawnerEntry->GetTabType()))
				{
					AllSpawners.AddUnique(SpawnerEntry);
				}
			}
		}

		return AllSpawners;
	}
};


template<auto M>
struct FExposedPrivate;

template<class T, class U, T U::*M>
struct FExposedPrivate<M> {
	friend T& ExposePrivate(U& u) {
		return u.*M;
	}
};

template struct FExposedPrivate<&FUICommandList::UICommandBindingMap>;
TMap< const TSharedPtr< const FUICommandInfo >, FUIAction >& ExposePrivate(FUICommandList&);


struct RecursiveCommandDataGetter final
{
	static TArray<TSharedPtr<IMenuItem>> CollectMenuItems()
	{
		TArray<TSharedPtr<IMenuItem>> Ret;
		for (auto& Pair : ExposePrivate(FEditorPlusUtils::GetLevelEditorModule().GetGlobalLevelEditorActions().Get()))
		{
			auto& Key = Pair.Key;
			auto& Value = Pair.Value;
			Ret.Push(MakeShared<FMenuItemCommand>(Key, Value));
		}

		for (auto& TabEntry: static_cast<FExposedGlobalTabmanager&>(FGlobalTabmanager::Get().Get()).CollectSpawners())
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
