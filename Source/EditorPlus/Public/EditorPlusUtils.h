// Copyright (c) 2023 - 2024, Disenone (https://github.com/disenone).
// All rights reserved. Licensed under MIT License.

#pragma once

#include "LevelEditor.h"

class EDITORPLUS_API FEditorPlusUtils
{
public:
	static FString GetPathDelimiter();
	
	static FLevelEditorModule& GetLevelEditorModule()
	{
		return FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	}

	static TArray<FString> SplitString(const FString& InString, const FString& InDelimiter = GetPathDelimiter());

	static FName PathJoin(const FName& Left, const FName& Right, const FString& InDelimiter = GetPathDelimiter());

	static FText CreateFText(const FString& InTextLiteral, const FString& InNamespace, const FString& InKey);

	static FName GenUniqueId(const FName& Tag);
};


// black magic to expose private members
// usage example:
//     using FExposedIconsType = TMap<FName, FSlateBrush*>;
//     EP_EXPOSE_PRIVATE(ExposedIcons, FSlateStyleSet, FExposedIconsType, BrushResources);
#define EP_EXPOSE_PRIVATE(Name, Class, MemberType, MemberName) \
	template<auto M> struct F##Name;	\
	template<class T, class U, T U::*M>	\
	struct F##Name<M> {					\
		friend T& Name(U& u) { return u.*M;	}	\
		friend const T& Name(const U& u) { return u.*M; }	\
	};									\
	template struct F##Name<&Class::MemberName>;	\
	MemberType& Name(Class&);	\
	const MemberType& Name(const Class&);
