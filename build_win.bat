echo on

set VER=%1

"e:\UE\Epic Games\UE_%VER%\Engine\Build\BatchFiles\RunUAT.bat" BuildPlugin -Plugin="e:\UE\projects\UE.EditorPlus\EditorPlus.uplugin" -Package="e:\UE\projects\products\EditorPlus\ue%VER%\EditorPlus" -Rocket
