# UE.EditorPlus

## 介绍

UnrealEngine 插件来方便扩展编辑器菜单。并增加一些实用的编辑器工具。

## 扩展编辑器菜单

支持路径简易扩展菜单，详细自定义扩展菜单。

### 路径简易扩展菜单

可以通过这样的方式来注册一个编辑器菜单指令

```cpp
FEditorPlusPath::RegisterPathAction(
    "/<MenuBar>Bar/<SubMenu>SubMenu/<Command>Action", 
    FExecuteAction::CreateLambda([]
		{
		    // do action
		})
);
```

这样就可以在编辑器菜单栏 Help 后面增加一个菜单栏 Bar，Bar 里面增加一个子菜单 SubMenu， SubMenu 里面增加一个命令 Action。

路径格式为 `/<Type1>Name1/<Type2>Name2`，目前支持的类型有：

    - <Hook>
    - <MenuBar>
    - <Section>
    - <Separator>
    - <SubMenu>
    - <Command>
    - <Widget>

更简易的路径形式：`/BarName/SubMenuName1/SubMenuName2/CommandName`，如果不制定类型，默认路径的第一个是 `<MenuBar>`，中间的是 `<SubMenu>`，最后的是 `<Command>`。

如果没有指定 `<Hook>` 则自动最前面加上 `<Hook>Help`，表示在 Help 菜单后面添加菜单栏。

完整的路径格式会是这样的：`/<Hook>HookName/<Type1>Name1/<Type2>Name2`，第一个路径必须是 `<Hook>`，后续的路径类型需要满足：

    - `<Hook>` 后面不能有 `<Hook>`
    - `<MenuBar>` 后面不能有 `<Hook>, <MenuBar>`
    - `<Section>` 后面不能有 `<Hook>, <MenuBar>, <Section>`
    - `<Separator>` 后面不能有 `<Hook>, <MenuBar>`
    - `<SubMenu>` 后面不能有 `<Hook>, <MenuBar>`
    - `<Command>` 后面不能有任何子路径
    - `<Widget>` 后面不能有任何子路径


### 详细自定义扩展菜单

详细自定义例子：

```cpp
EP_NEW_MENU(FEditorPlusMenuBar)("Bar")
->RegisterPath()
->Content({
    EP_NEW_MENU(FEditorPlusSubMenu)("SubMenu")
    ->Content({
        EP_NEW_MENU(FEditorPlusCommand)("Action")
        ->BindAction(FExecuteAction::CreateLambda([]
            {
                // do action
            })),
    })
});
```

同样生成一个路径为 `/Bar/SubMenu/Action` 的菜单指令。

### 混合方式

可以混合实用路径方式和自定义方式：

```cpp
FEditorPlusPath::RegisterPath(
    "/<MenuBar>Bar/<SubMenu>SubMenu/<Command>Action", 
    EP_NEW_MENU(FEditorPlusCommand)("Action")
    ->BindAction(FExecuteAction::CreateLambda([]
        {
            // do action
        })),
);
```

混合方式可以自定义最后路径的具体设置。

### 接口说明
... to be continue

### 一些额外的设定
... to be continue

## TODO

- ~~支持设置字体大小~~
- ~~搜索优化（排序规则优化）~~
- ~~cache 每次打开都清~~
- ~~优化输入框相应体验~~
- ~~解决 uniid 问题~~
- ~~增加功能支持在编辑器原来的菜单栏里面加菜单~~
- ~~支持多语言~~
- 增加工具 收藏菜单
- pathmenu 支持扩展工具栏
