@ECHO OFF

IF NOT "%~1"=="release" IF NOT "%~1"=="debug" (
    echo Invalid configuration - must be debug/release 1>&2
    exit /b
)

pushd %~dp0 || exit /b

set CMAKE=%~dp0..\..\..\external\cmake\bin\cmake
%CMAKE% --build ..\..\..\build\playstation --config %~1 || goto done

:done
popd