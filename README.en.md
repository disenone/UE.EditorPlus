# UE.EditorPlus

[English](README.en.md) | [简体中文](README.md)

## Introduction

UE.EditorPlus is a plugin for Unreal Engine that facilitates the extension of the editor menu and adds some practical editor tools.

https://github.com/disenone/UE.EditorPlus/assets/2453074/9d719361-d0b1-4c7c-ba71-d862f404adfb

## Detailed Documentation

中文：[UE 编辑器插件 UE.EditorPlus 说明文档](https://disenone.github.io/wiki/ue-%E7%BC%96%E8%BE%91%E5%99%A8%E6%8F%92%E4%BB%B6-EditorPlus/)

English: [UE Editor Plugin UE.EditorPlus Documentation](https://disenone.github.io/wiki/en/ue-%E7%BC%96%E8%BE%91%E5%99%A8%E6%8F%92%E4%BB%B6-EditorPlus/)

## Extending the Editor Menu / Toolbar

Supports path-based extension, instantiated extension, and mixed extension.

```cpp
FEditorPlusPath::RegisterPathAction(
    "/<MenuBar>Bar/<SubMenu>SubMenu/<Command>Action",
    FExecuteAction::CreateLambda([]
        {
            // perform action
        })
);
```

![](https://disenone.github.io/wiki/assets/img/2024-ue-editorplus/menu.png)

![](https://disenone.github.io/wiki/assets/img/2024-ue-editorplus/toolbar.png)

## Practical Tools

### ClassBrowser

![](https://disenone.github.io/wiki/assets/img/2024-ue-editorplus/classbrowser_menu.png)

![](https://disenone.github.io/wiki/assets/img/2024-ue-editorplus/classbrowser.png)

### MenuCollection

Quickly find and collect editor menu commands

![](https://disenone.github.io/wiki/assets/img/2024-ue-editorplus/menucollection_find.png)

![](https://disenone.github.io/wiki/assets/img/2024-ue-editorplus/menucollection_star.png)
