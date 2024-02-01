echo off

set CWD=%~dp0
set ENGINE=e:\UE\Epic Games\UE_
set OUTPUT=e:\UE\projects\products
set VERS=5.0 5.1 5.2 5.3
set ZIP="D:\Program Files\7-Zip\7z.exe"

set ARG=%1

if "%ARG%"=="" (
    goto :help
)

@REM check version valid
echo %VERS% | findstr %ARG% >nul && (
    @REM echo %ARG% valid
) || (
    if "%ARG%" neq "all" (
        echo Engine Version [%ARG%] invalid
        goto :help
    )
)

if "%ARG%"=="all" (
    (for %%v in (%VERS%) do ( 
        call :package %%v
    ))
) else (
    call :package %ARG%
)

goto :exit

:package
    set VER=%~1
    echo "--------------------Building Version [%VER%]--------------------"
    call "%ENGINE%%VER%\Engine\Build\BatchFiles\RunUAT.bat" BuildPlugin -Plugin="%CWD%EditorPlus.uplugin" -Package="%OUTPUT%\EditorPlus\ue%VER%\EditorPlus" -Rocket
    mkdir "%OUTPUT%\EditorPlus\ue%VER%\EditorPlus\Config"
    xcopy /y "%CWD%\Config\FilterPlugin.ini" "%OUTPUT%\EditorPlus\ue%VER%\EditorPlus\Config\"
    rmdir /s /q %OUTPUT%\EditorPlus\ue%VER%\EditorPlus\Binaries"
    rmdir /s /q %OUTPUT%\EditorPlus\ue%VER%\EditorPlus\Intermediate"
    del /q "%OUTPUT%\EditorPlus\ue%VER%\*.zip" > nul
    call %ZIP% a -tzip "%OUTPUT%\EditorPlus\ue%VER%\EditorPlus_ue%VER%.zip" "%OUTPUT%\EditorPlus\ue%VER%\EditorPlus" > nul
    xcopy /y "%OUTPUT%\EditorPlus\ue%VER%\EditorPlus_ue%VER%.zip" "%OUTPUT%\EditorPlus\"
goto :eof

:help
    echo "Usage: build_win.bat [all|5.0|5.1|5.2|5.3]"

:exit
