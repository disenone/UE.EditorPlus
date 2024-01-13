# UE.EditorPlus

[English](README.en.md) | [简体中文](README.md)

## 介绍

UnrealEngine 插件来方便扩展编辑器菜单。并增加一些实用的编辑器工具。

## 详细文档

中文：[UE 编辑器插件 UE.EditorPlus 说明文档](https://disenone.github.io/wiki/ue-%E7%BC%96%E8%BE%91%E5%99%A8%E6%8F%92%E4%BB%B6-EditorPlus/)

English：[UE Editor Plugin UE.EditorPlus Documentation](https://disenone.github.io/wiki/en/ue-%E7%BC%96%E8%BE%91%E5%99%A8%E6%8F%92%E4%BB%B6-EditorPlus/)

## 扩展编辑器菜单

支持路径扩展菜单，实例化扩展菜单，混合扩展菜单。

```cpp
FEditorPlusPath::RegisterPathAction(
    "/<MenuBar>Bar/<SubMenu>SubMenu/<Command>Action",
    FExecuteAction::CreateLambda([]
        {
            // do action
        })
);
```

![](Resources/menu.png)

## 实用工具

### ClassBrowser

![](Resources/classbrowser.png)


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
