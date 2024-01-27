echo on

set CWD=%~dp0
set ENGINE=e:\UE\Epic Games\UE_
set OUTPUT=e:\UE\projects\products
set VERS=5.0 5.1 5.2 5.3

set ARG=%1

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
    call "%ENGINE%%VER%\Engine\Build\BatchFiles\RunUAT.bat" BuildPlugin -Plugin="%CWD%EditorPlus.uplugin" -Package="%OUTPUT%\EditorPlus\ue%VER%\EditorPlus" -Rocket
    mkdir "%OUTPUT%\EditorPlus\ue%VER%\EditorPlus\Config"
    xcopy /y "%CWD%\Config\FilterPlugin.ini" "%OUTPUT%\EditorPlus\ue%VER%\EditorPlus\Config\"
    rmdir /s /q %OUTPUT%\EditorPlus\ue%VER%\EditorPlus\Binaries"
    rmdir /s /q %OUTPUT%\EditorPlus\ue%VER%\EditorPlus\Intermediate"

goto :eof

:help
    echo "Usage: build_win.bat [all|5.0|5.1|5.2|5.3]"

:exit
