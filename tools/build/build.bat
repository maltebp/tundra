@ECHO OFF

IF NOT "%~1"=="release" IF NOT "%~1"=="debug" (
    echo Invalid configuration - must be debug/release 1>&2
    exit /b
)

pushd %~dp0 || exit /b

cmake --preset %~1 || goto done
cmake --build %~dp0..\..\build\engine\%~1 || goto done

:done
popd

:missing-args