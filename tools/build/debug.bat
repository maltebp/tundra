@ECHO OFF

pushd %~dp0
cmake --preset debug || (popd & exit /b)

pushd %~dp0..\..\
cmake --build build
popd

popd