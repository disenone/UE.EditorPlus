
#pragma once

#include <Widgets/SCompoundWidget.h>
#include <Widgets/Views/STileView.h>
#include "SoundInfo.h"

class SMultiLineEditableText;

class SSoundBrowserTab final : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SSoundBrowserTab)
	{}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
	void OnClose() {};

	using FSoundType = TSharedRef<FSoundInfo>;
	using FSoundListType = TArray<FSoundType>;

private:
	TSharedRef<SWidget> ConstructContent();
	TSharedRef<ITableRow> OnSoundTile(FSoundType InItem, const TSharedRef<STableViewBase>& OwnerTable);
	FReply OnClickSound(const FSoundType& InItem);
	void OnHoverSound(const FSoundType& InItem);
	void OnUnhoverSound(const FSoundType& InItem);
	FReply OnResetSound();
	FReply OnSelectSound(const FString& InAction, const FSoundType& InSound);
	FReply OnPlaySound();

	void OnSearchSound(const FText& InFilterText);
	void UpdateSoundList();

private:
	FSoundListType					SoundAllList;
	FSoundListType					SoundList;
	typedef STileView<FSoundType>	SSoundView;
	TSharedPtr<SSoundView>			SoundView;

	TSharedPtr<SSearchBox>			SoundSearchBox;
	TSharedPtr<FActiveTimerHandle>	SoundSearchTimer;
	const float						SoundSearchDelay = 0.1f;

	// usage
	TSharedPtr<FSoundInfo>			SelectedSound;
	TSharedPtr<SMultiLineEditableText>		SelectedSoundName;
	TSharedPtr<STextBlock>			SelectSoundText;
	TSharedPtr<SMultiLineEditableText>		DetailText;
};
