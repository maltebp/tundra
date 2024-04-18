@ECHO OFF

pushd %~dp0 || exit /b

set CMAKE=%~dp0..\..\..\external\cmake\bin\cmake
%CMAKE% --preset developer

popd