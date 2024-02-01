# UE.EditorPlus

[English](README.en.md) | [简体中文](README.md)

## 介绍

UnrealEngine 插件来方便扩展编辑器菜单。并增加一些实用的编辑器工具。

https://github.com/disenone/UE.EditorPlus/assets/2453074/9d719361-d0b1-4c7c-ba71-d862f404adfb

## 详细文档

中文：[UE 编辑器插件 UE.EditorPlus 说明文档](https://disenone.github.io/wiki/ue-%E7%BC%96%E8%BE%91%E5%99%A8%E6%8F%92%E4%BB%B6-EditorPlus/)

English：[UE Editor Plugin UE.EditorPlus Documentation](https://disenone.github.io/wiki/en/ue-%E7%BC%96%E8%BE%91%E5%99%A8%E6%8F%92%E4%BB%B6-EditorPlus/)

## 扩展编辑器菜单/工具栏

支持路径扩展，实例化扩展，混合扩展。

```cpp
FEditorPlusPath::RegisterPathAction(
    "/<MenuBar>Bar/<SubMenu>SubMenu/<Command>Action",
    FExecuteAction::CreateLambda([]
        {
            // do action
        })
);
```

![](https://disenone.github.io/wiki/assets/img/2024-ue-editorplus/menu.png)

![](https://disenone.github.io/wiki/assets/img/2024-ue-editorplus/toolbar.png)

## 实用工具

### ClassBrowser

类浏览器

![](https://disenone.github.io/wiki/assets/img/2024-ue-editorplus/classbrowser_menu.png)

![](https://disenone.github.io/wiki/assets/img/2024-ue-editorplus/classbrowser.png)

### MenuCollection

编辑器菜单命令快速查找和收藏

![](https://disenone.github.io/wiki/assets/img/2024-ue-editorplus/menucollection_find.png)

![](https://disenone.github.io/wiki/assets/img/2024-ue-editorplus/menucollection_star.png)


## TODO

- ~~支持设置字体大小~~
- ~~搜索优化（排序规则优化）~~
- ~~cache 每次打开都清~~
- ~~优化输入框相应体验~~
- ~~解决 uniid 问题~~
- ~~增加功能支持在编辑器原来的菜单栏里面加菜单~~
- ~~支持多语言~~
- ~~支持指定 parent 和 path 来扩展菜单~~
- 增加工具 收藏菜单
- pathmenu 支持扩展工具栏
