@ECHO OFF

pushd %~dp0 || exit /b

cmake --preset playstation

popd