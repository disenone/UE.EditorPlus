// Copyright (c) 2023 - 2024, Disenone (https://github.com/disenone).
// All rights reserved. Licensed under MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Brushes/SlateColorBrush.h"

namespace ClassBrowser_Detail
{
	
enum class EClassType: uint32
{
	None = 0,
	Simple,
	Class,
	ScriptStruct,
	Enum,
	Property,
	Function,
};
	
class FClassStyle
{
public:
	static TSharedPtr<FClassStyle> Get();
	FLinearColor GetColorByType(EClassType Type);
	
	FSlateColorBrush BorderBackground;
	FSlateColorBrush BorderCurrentClass;
	TMap<EClassType, FLinearColor> ColorMap;
	TSharedPtr<FTableRowStyle> TableRowStyle;
private:
	FClassStyle();
	template <typename ObjectType, ESPMode Mode> friend class SharedPointerInternals::TIntrusiveReferenceController;
};

	
class FClassItemBase
{
public:
	FClassItemBase() {}
	virtual ~FClassItemBase() {}
	
	virtual FString GetName() const { return TEXT(""); };
	virtual FString GetPathName() const { return GetName(); };
	virtual FString GetDesc() const { return TEXT(""); };
	virtual FLinearColor GetColor() const { return FClassStyle::Get()->GetColorByType(GetType()); }
	virtual bool CanNavigateToSource() const { return false; }
	virtual void NavigateToSource() {}
	virtual const void* GetItem() const = 0;
	virtual EClassType GetType() const  = 0;
	virtual TArray<TSharedPtr<FClassItemBase>> GetItemInfoList() = 0;
	virtual TSharedPtr<FClassItemBase> MakeOtherShared(const UObject* Item) = 0;
	virtual bool operator==(const FClassItemBase& Other) const
	{
		return GetType() == Other.GetType() && GetPathName() == Other.GetPathName();
	}
};

class FClassItemSimple: public FClassItemBase
{
public:
	explicit FClassItemSimple(const FString& Name=TEXT(""), const FString& Desc=TEXT(""), const FLinearColor& Color=FLinearColor::Black):
		FClassItemBase(), Name(Name), Desc(Desc), Color(Color) {}
	virtual FString GetName() const override { return Name; }
	virtual FString GetDesc() const override { return Desc; }
	virtual FLinearColor GetColor() const override { return Color; }
	virtual const void* GetItem() const override { return nullptr; }
	virtual TArray<TSharedPtr<FClassItemBase>> GetItemInfoList() override { return {}; }
	virtual EClassType GetType() const override { return EClassType::Simple; };
	virtual TSharedPtr<FClassItemBase> MakeOtherShared(const UObject* Item) override { return {}; };
	
public:
	FString Name;
	FString Desc;
	FLinearColor Color;
};
	
template <class ItemClass>
class TClassItem: public FClassItemBase
{
public:
	explicit TClassItem(const ItemClass* Item): FClassItemBase(), Item(Item) {}

	virtual FString GetName() const override { return Item->GetName(); }
	virtual FString GetPathName() const override { return Item->GetPathName(); }
	virtual const void* GetItem() const override { return static_cast<const void*>(Item); }
	
	virtual bool operator==(const FClassItemBase& Other) const override
	{
		return GetType() == Other.GetType() && GetItem() == Other.GetItem();
	}
public:
	const ItemClass* Item;
};

#define CLASS_ITEM(Class) \
	class FClassItem##Class: public TClassItem<Class> \
	{ \
	private: \
		using Super = TClassItem<Class>; \
	public: \
		explicit FClassItem##Class(const Class* Item): Super(Item) {} \
		virtual FString GetName() const override; \
		virtual FString GetDesc() const override; \
		virtual bool CanNavigateToSource() const override; \
		virtual void NavigateToSource() override; \
		virtual TArray<TSharedPtr<FClassItemBase>> GetItemInfoList() override; \
	};

#define CLASS_TYPE_ITEM(Type, Class) \
	class FClassItem##Type: public FClassItem##Class \
	{ \
	private: \
		using Super = FClassItem##Class; \
	public: \
		explicit FClassItem##Type(const Class* Item): Super(Item) {} \
		virtual EClassType GetType() const override { return EClassType::Type; }; \
		virtual TSharedPtr<FClassItemBase> MakeOtherShared(const UObject* _Item) override { return MakeShared<FClassItem##Type>((const Class*)_Item); } \
	};

CLASS_ITEM(UStruct)
CLASS_ITEM(FProperty)
CLASS_ITEM(UFunction)
CLASS_ITEM(UEnum)
	
CLASS_TYPE_ITEM(Class, UStruct)
CLASS_TYPE_ITEM(ScriptStruct, UStruct)
CLASS_TYPE_ITEM(Property, FProperty)
CLASS_TYPE_ITEM(Function, UFunction)
CLASS_TYPE_ITEM(Enum, UEnum)

#undef CLASS_ITEM
#undef CLASS_TYPE_ITEM
	
}
