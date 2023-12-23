#include "EditorPlusPathMenuManager.h"
#include "EditorPlusUtils.h"


TArray<FString> CheckAndSplitPath(const FName& Path, const int32 MinLen = 0)
{
	checkf(!Path.IsNone(), TEXT("Path is None"));

	const FString SPath = Path.ToString();

	TArray<FString> Split = FEditorPlusUtils::SplitString(SPath, "/");

	checkf(Split.Num() >= MinLen, TEXT("Path must have at least %d Length, ag: /a/b"), MinLen);

	return Split;
}


TArray<FString> FEditorPlusPathMenuManager::NormalizeSplitPath(const FString& Path)
{
	if(Path == "") return TArray<FString>();

	const FString Delimiter = FEditorPlusUtils::GetPathDelimiter();

	TArray<FString> Split = FEditorPlusUtils::SplitString(Path, Delimiter);
	if(Split.IsEmpty()) return TArray<FString>();
	
	TArray<FString> NormalizedNames;

	for(size_t i = 0; i < Split.Num(); ++i)
	{
		const auto& Name = Split[i];
		if(Name.StartsWith(FEditorPlusMenuBase::PathTagLeft) && Name.Find(FEditorPlusMenuBase::PathTagRight) != INDEX_NONE)
		{
			const FString PathTag = Name.Left(Name.Find(">") + 1);
			if(FEditorPlusMenuBase::GetPathTagType(PathTag)._to_integral() == EEditorPlusMenuType::None)
			{
				UE_LOG(LogEditorPlus, Error, TEXT("Invalid Path Tag [%s] in Path [%s]"), ToCStr(PathTag), ToCStr(Path));
				return TArray<FString>();
			}
			
			NormalizedNames.Push(Name);
		}
		else
		{
			if(i == 0) NormalizedNames.Push(FEditorPlusMenuBase::GetTypePathTag(EEditorPlusMenuType::MenuBar) + Name);
			else if(i == Split.Num() - 1) NormalizedNames.Push(FEditorPlusMenuBase::GetTypePathTag(EEditorPlusMenuType::Command) + Name);
			else NormalizedNames.Push(FEditorPlusMenuBase::GetTypePathTag(EEditorPlusMenuType::SubMenu) + Name);
		}
	}

	if(!NormalizedNames[0].StartsWith(FEditorPlusMenuBase::GetTypePathTag(EEditorPlusMenuType::Hook)))
		NormalizedNames.Insert(FEditorPlusMenuBase::GetTypePathTag(EEditorPlusMenuType::Hook) + "Help", 0);

	return NormalizedNames;
}


FString FEditorPlusPathMenuManager::NormalizePath(const FString& Path)
{
	const auto NormalizedNames = NormalizeSplitPath(Path);

	if(NormalizedNames.IsEmpty()) return "";
	
	const auto Delimiter = FEditorPlusUtils::GetPathDelimiter();
	return Delimiter + FString::Join(NormalizedNames, ToCStr(Delimiter));
}


TSharedPtr<FEditorPlusHook> FEditorPlusPathMenuManager::MakeRoot(const FString& PathName)
{
	const auto TypeAneName = FEditorPlusMenuBase::GetTypeAndNameByPathName(PathName);
	
	if(TypeAneName.Get<0>()._to_integral() != EEditorPlusMenuType::Hook)
	{
		UE_LOG(LogEditorPlus, Error, TEXT("Invalid Root Path: [%s], need to be <Hook>xxx"), ToCStr(PathName));
		return nullptr;
	}
	
	if (RootMap.Contains(PathName)) return RootMap[PathName];

	const FString& Name = TypeAneName.Get<1>();
	auto Hook = NEW_EP_MENU(FEditorPlusHook)(FName(Name));
	RootMap.Add(PathName, Hook);
	return Hook;
}


TSharedPtr<FEditorPlusMenuBase> FEditorPlusPathMenuManager::MakeNode(
	const TSharedRef<FEditorPlusMenuBase>& Parent, const FString& PathName, bool IsLeaf,
	const TSharedPtr<FEditorPlusMenuBase>& Node, const FText& FriendlyName, const FText& FriendlyTips)
{
	if(Node.IsValid())
	{
		const auto TypeAneName = FEditorPlusMenuBase::GetTypeAndNameByPathName(PathName);
		if(TypeAneName.Get<0>()._to_integral() != Node->GetType())
		{
			UE_LOG(
				LogEditorPlus, Error, TEXT("PathName dose not match Node Type, PathName: [%s], Node Type: [%s]"),
				ToCStr(PathName), StringCast<TCHAR>(Node->GetType()._to_string()).Get());
			return nullptr;
		}
		
		if(!Parent->AddChild(Node.ToSharedRef())) return nullptr;
		return Node;	
	}
	
	TArray<TSharedRef<FEditorPlusMenuBase>> Subs = Parent->GetChildrenByPathName(PathName);

	if (!Subs.Num())
	{
		const TSharedPtr<FEditorPlusMenuBase> NewNode = FEditorPlusMenuBase::CreateByPathName(PathName, FriendlyName, FriendlyTips);
		if(!NewNode.IsValid()) return nullptr;

		const auto Role = EEditorPlusMenuRole::_from_integral(IsLeaf ? EEditorPlusMenuRole::Leaf : EEditorPlusMenuRole::Node);
		
		if(!NewNode->AllowRole(Role))
		{
			UE_LOG(
				LogEditorPlus, Error, TEXT("Role not allow: [%s] can not be [%s]"),
				ToCStr(PathName), StringCast<TCHAR>(Role._to_string()).Get());
			return nullptr;
		}

		if(!Parent->AddChild(NewNode.ToSharedRef())) return nullptr;
		return NewNode;
	}
	
	return Subs[0];
}


TSharedPtr<FEditorPlusMenuBase> FEditorPlusPathMenuManager::DoRegister(
	const TArray<FString>& NormalizedPathNames, const TSharedPtr<FEditorPlusMenuBase>& Node, const FText& FriendlyName, const FText& FriendlyTips)
{
	if (NormalizedPathNames.Num() < 2)
	{
		UE_LOG(
			LogEditorPlus, Error, TEXT("Invalid Path: [%s]"),
			ToCStr(FEditorPlusUtils::GetPathDelimiter() + FString::Join(NormalizedPathNames, ToCStr(FEditorPlusUtils::GetPathDelimiter()))));
		return nullptr;	
	}
	
	// Root
	const TSharedPtr<FEditorPlusHook> Root = MakeRoot(NormalizedPathNames[0]);
	if(!Root.IsValid()) return nullptr;
	
	TSharedPtr<FEditorPlusMenuBase> Parent = Root;
	
	// Build Node
	for (int i = 1; i < NormalizedPathNames.Num() - 1; ++i)
	{
		Parent = MakeNode(Parent.ToSharedRef(), NormalizedPathNames[i], false);
		if(!Parent.IsValid()) return nullptr;
	}

	// Build Last Node
	auto Ret = MakeNode(
		Parent.ToSharedRef(), NormalizedPathNames[NormalizedPathNames.Num() - 1], true, Node, FriendlyName, FriendlyTips);

	// Register to LevelEditor
	Root->AddExtension();

	return Ret;
}


TSharedPtr<FEditorPlusMenuBase> FEditorPlusPathMenuManager::RegisterPath(const FString& Path, const TSharedPtr<FEditorPlusMenuBase>& Menu)
{
	checkf(!Menu->IsDestroyed(), TEXT("Menu is alreasy destroyed, can not register again."));
	return DoRegister(Path, Menu);	
}


TSharedPtr<FEditorPlusMenuBase> FEditorPlusPathMenuManager::RegisterPath(const FString& Path, const FText& FriendlyName, const FText& FriendlyTips)
{
	return DoRegister(Path, nullptr, FriendlyName, FriendlyTips);
}


TSharedPtr<FEditorPlusMenuBase> FEditorPlusPathMenuManager::RegisterAction(
	const FString& Path, const FExecuteAction& ExecuteAction, const FName& Hook, const FText& FriendlyName, const FText& FriendlyTips)
{
	const auto NormalizedNames = NormalizeSplitPath(Path);

	const auto TypeAneName = FEditorPlusMenuBase::GetTypeAndNameByPathName(NormalizedNames[NormalizedNames.Num() - 1]);

	if(TypeAneName.Get<0>()._to_integral() != EEditorPlusMenuType::Command)
	{
		UE_LOG(LogEditorPlus, Error, TEXT("Invalid Path [%s] for RegisterAction, Path Leaf need to be <Command>xxx"), ToCStr(Path));
		return nullptr;
	}

	const FName Name = FName(TypeAneName.Get<1>());
	const auto Command = NEW_EP_MENU(FEditorPlusCommand)(Name, Hook.IsNone() ? Name : Hook, FriendlyName, FriendlyTips)
		->BindAction(ExecuteAction);
	
	return DoRegister(NormalizedNames, Command);
}


bool FEditorPlusPathMenuManager::RemoveNode(
	const TSharedRef<FEditorPlusMenuBase>& Parent, const TArray<FString>& Paths,
	const int ParentIdx, const TSharedPtr<FEditorPlusMenuBase>& Leaf)
{
	// DFS Remove Node
	if (ParentIdx >= Paths.Num() - 1) return false;

	const FString& ChildName = Paths[ParentIdx + 1];

	if(ParentIdx == Paths.Num() - 2)
	{
		if(Leaf.IsValid())
		{
			return Parent->RemoveChild(Leaf.ToSharedRef());
		}
		
		return Parent->RemoveChildByPathName(ChildName);
	}
	
	for(const auto& Child: Parent->GetChildrenByPathName(ChildName))
	{
		if(RemoveNode(Child, Paths, ParentIdx + 1, Leaf))
		{
			if(Child->IsEmpty())
			{
				Child->Destroy();
				Parent->RemoveChild(Child);
			}
			return true;
		}
	}

	return false;
}


bool FEditorPlusPathMenuManager::RemoveNode(
	const TSharedRef<FEditorPlusMenuBase>& Parent, const TArray<FString>& Paths,
	const int ParentIdx, const FName& UniqueId)
{
	// DFS Remove Node
	if (ParentIdx >= Paths.Num() - 1) return false;

	const FString& ChildName = Paths[ParentIdx + 1];

	if(ParentIdx == Paths.Num() - 2)
	{
		return Parent->RemoveChildByUniqueId(UniqueId);
	}
	
	for(const auto& Child: Parent->GetChildrenByPathName(ChildName))
	{
		if(RemoveNode(Child, Paths, ParentIdx + 1, UniqueId))
		{
			if(Child->IsEmpty())
			{
				Child->Unregister();
				Parent->RemoveChild(Child);
			}
			return true;
		}
	}

	return false;	
}


bool FEditorPlusPathMenuManager::Unregister(const FString& Path, const TSharedPtr<FEditorPlusMenuBase>& Leaf)
{
	const auto PathNames = NormalizeSplitPath(Path);
	if(PathNames.IsEmpty()) return false;

	const FString& RootName = PathNames[0];
	if (!RootMap.Contains(RootName)) return false;

	if(RemoveNode(RootMap[RootName], PathNames, 0, Leaf))
	{
		if(RootMap[RootName]->IsEmpty())
		{
			RootMap[RootName]->Destroy();
			RootMap.Remove(RootName);
		}
		return true;
	}

	return false;
}


bool FEditorPlusPathMenuManager::Unregister(const FString& Path, const FName& UniqueId)
{
	const auto PathNames = NormalizeSplitPath(Path);
	if(PathNames.IsEmpty()) return false;

	const FString& RootName = PathNames[0];
	if (!RootMap.Contains(RootName)) return false;

	if(RemoveNode(RootMap[RootName], PathNames, 0, UniqueId))
	{
		if(RootMap.Contains(RootName) && RootMap[RootName]->IsEmpty())
		{
			RootMap[RootName]->Unregister();	
			RootMap.Remove(RootName);
		}
		return true;
	}

	return false;	
}


TSharedPtr<FEditorPlusMenuBase> FEditorPlusPathMenuManager::GetNodeByPath(const FString& Path)
{
	const auto PathNames = NormalizeSplitPath(Path);
	if(PathNames.IsEmpty()) return nullptr;

	const FString& RootName = PathNames[0];
	if(!RootMap.Contains(RootName)) return nullptr;
	
	TSharedRef<FEditorPlusMenuBase> CurrentNode = RootMap[RootName];

	for (int i = 1; i < PathNames.Num(); ++i)
	{
		const auto Subs = CurrentNode->GetChildrenByPathName(PathNames[i]);

		if(!Subs.Num()) return nullptr;

		CurrentNode = Subs[0];
	}
	return CurrentNode;
}
