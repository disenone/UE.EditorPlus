#include "ClassItem.h"
#include "SourceCodeNavigation.h"

namespace ClassBrowser_Detail
{
// ClassStyle
FClassStyle::FClassStyle(): BorderBackground(FLinearColor::Gray), BorderCurrentClass(FLinearColor::White)
{
	ColorMap = TMap<EClassType, FLinearColor>{
		{EClassType::Simple, FLinearColor::White},
		{EClassType::Class, FLinearColor::White},
		{EClassType::Property, FLinearColor(0.7, 0.7, 1)},
		{EClassType::Function, FLinearColor(1.0, 0.7, 0.7)},
		{EClassType::Enum, FLinearColor::White},
		{EClassType::ScriptStruct, FLinearColor::White},
	};

	TableRowStyle = MakeShared<FTableRowStyle>(FCoreStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row"));
	TableRowStyle->InactiveBrush = TableRowStyle->ActiveBrush;
	TableRowStyle->InactiveHoveredBrush = TableRowStyle->ActiveHoveredBrush;
}

TSharedPtr<FClassStyle> FClassStyle::Get()
{
	static TSharedPtr<FClassStyle> Instance = MakeShared<FClassStyle>();
	return Instance;
}

FLinearColor FClassStyle::GetColorByType(EClassType Type)
{
	const FLinearColor* Color = ColorMap.Find(Type);
	if (Color)
	{
		return *Color;
	}
	return FLinearColor::Black;	
}


bool ClassItemSort(TSharedPtr<FClassItemBase> ItemA, TSharedPtr<FClassItemBase> ItemB)
{
	
	if (ItemA->GetType() != ItemB->GetType())
	{
		return ItemA->GetType() < ItemB->GetType();
	}
	return ItemA->GetName() < ItemB->GetName();
}
	
// UClass
FString FClassItemUStruct::GetName() const
{
	return Item->GetPrefixCPP() + Item->GetName();
}

FString FClassItemUStruct::GetDesc() const
{
	return Item->GetFullName() + TEXT("\n\n") + Item->GetToolTipText().ToString();
}

bool FClassItemUStruct::CanNavigateToSource() const
{
	switch (GetType())
	{
	case EClassType::Class:
		return FSourceCodeNavigation::CanNavigateToClass(static_cast<const UClass*>(Item));
	case EClassType::ScriptStruct:
		return FSourceCodeNavigation::CanNavigateToStruct(static_cast<const UScriptStruct*>(Item));
	default:
		break;
	}
	return false;
}

void FClassItemUStruct::NavigateToSource()
{
	switch (GetType())
	{
	case EClassType::Class:
		FSourceCodeNavigation::NavigateToClass(static_cast<const UClass*>(Item));
		break;
	case EClassType::ScriptStruct:
		FSourceCodeNavigation::NavigateToStruct(static_cast<const UScriptStruct*>(Item));
		break;
	default:
		break;
	}
}

TArray<TSharedPtr<FClassItemBase>> FClassItemUStruct::GetItemInfoList()
{
	TArray<TSharedPtr<FClassItemBase>> Result;
	for (const UStruct* Super = Item->GetSuperStruct(); Super; Super = Super->GetSuperStruct())
	{
		switch (GetType())
		{
		case EClassType::Class:
			Result.Push(MakeShared<FClassItemClass>(Super));
			break;
		case EClassType::ScriptStruct:
			Result.Push(MakeShared<FClassItemScriptStruct>(Super));
			break;
		default:
			break;
		}
	}
	
	for (TFieldIterator<FProperty> Prop(Item); Prop; ++Prop)
	{
		Result.Push(MakeShared<FClassItemProperty>(*Prop));
	}
	
	for (TFieldIterator<UFunction> Func(Item); Func; ++Func)
	{
		Result.Push(MakeShared<FClassItemFunction>(*Func));
	}
	Result.Sort(ClassItemSort);

	return Result;	
}

	
// FProperty

FString FClassItemFProperty::GetName() const
{
	return Item->GetNameCPP();
}

FString FClassItemFProperty::GetDesc() const
{
	return Item->GetFullName() + TEXT("\n\n") + Item->GetToolTipText().ToString();
}

bool FClassItemFProperty::CanNavigateToSource() const
{
	return FSourceCodeNavigation::CanNavigateToProperty(Item);
}

void FClassItemFProperty::NavigateToSource()
{
	FSourceCodeNavigation::NavigateToProperty(Item);
}

TArray<TSharedPtr<FClassItemBase>> FClassItemFProperty::GetItemInfoList()
{
	return {};
}
	
// UFunction

FString FClassItemUFunction::GetName() const
{
	return Item->GetName();
}

	
FString FClassItemUFunction::GetDesc() const
{
	TArray<FString> Signatures{TEXT("void"), TEXT(" "), Item->GetName(), TEXT("(")};
	TArray<FString> Args;
	for (TFieldIterator<FProperty> Arg(Item); Arg; ++Arg)
	{
		if (Arg->PropertyFlags & CPF_ReturnParm)
		{
			Signatures[0] = Arg->GetCPPType();
		}
		else
		{
			Args.Push(Arg->GetCPPType() + TEXT(" ") + Arg->GetNameCPP());
		}
	}
	if (Args.Num())
	{
		Signatures.Push(FString::Join(Args, TEXT(", ")));
	}
	Signatures.Push(TEXT(")"));	
	const TArray<FString> Desc{Item->GetFullName(), Item->GetToolTipText().ToString(), FString::Join(Signatures, TEXT(""))};
	return FString::Join(Desc, TEXT("\n\n"));	
}

bool FClassItemUFunction::CanNavigateToSource() const
{
	return FSourceCodeNavigation::CanNavigateToFunction(Item);
}

void FClassItemUFunction::NavigateToSource()
{
	FSourceCodeNavigation::NavigateToFunction(Item);
}

TArray<TSharedPtr<FClassItemBase>> FClassItemUFunction::GetItemInfoList()
{
	return {};
}
	
// UEnum
FString FClassItemUEnum::GetName() const
{
	return Item->GetName();
}
	
FString FClassItemUEnum::GetDesc() const
{
	return Item->GetFullName();
}

bool FClassItemUEnum::CanNavigateToSource() const
{
	return true;	
}

void FClassItemUEnum::NavigateToSource()
{
	FSourceCodeNavigation::NavigateToClass((const UClass*)(Item));	
}

TArray<TSharedPtr<FClassItemBase>> FClassItemUEnum::GetItemInfoList()
{
	TArray<TSharedPtr<FClassItemBase>> Result;
	Result.Reserve(Item->NumEnums());
	
	for (int Index=0; Index < Item->NumEnums(); ++Index)
	{
		Result.Push(MakeShared<FClassItemSimple>(
			Item->GetNameStringByIndex(Index),
			Item->GetDisplayNameTextByIndex(Index).ToString() + TEXT("\n\n") + Item->GetToolTipTextByIndex(Index).ToString(),
			GetColor()));
	}

	return Result;
}
	
}
