@ECHO OFF

pushd %~dp0
cmake --preset release || (popd & exit /b)

pushd %~dp0..\..\
cmake --build build
popd

popd