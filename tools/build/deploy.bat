echo %~dp0
set root=%~dp0..\..

pushd %~dp0 || exit /b

echo Building...
call developer\generate.bat || goto :error
call developer\build.bat debug || goto :error
call developer\build.bat release || goto :error

call playstation\generate.bat || goto :error
call playstation\build.bat debug || goto :error
call playstation\build.bat release || goto :error

echo Deploying...

set deploy_dir=%root%\build\deploy

rm -rf %deploy_dir%
mkdir %deploy_dir%

xcopy "%root%\build\developer\bin\Release\td-asset-compiler.exe" "%deploy_dir%\bin\release\" /q > nul

xcopy "%root%\build\playstation\lib\Debug\" "%deploy_dir%\lib\debug\" /q > nul
xcopy "%root%\build\playstation\lib\Release\" "%deploy_dir%\lib\release\" /q > nul

xcopy "%root%\include\" "%deploy_dir%\include\" /s /q > nul

xcopy "%root%\external" %deploy_dir%\external\ /s /q > nul

xcopy "%root%\tools\cmake\" %deploy_dir%\tools\cmake\ /s /q > nul

popd

echo Successfully deployed

exit /b 0

:error
popd
exit /b -1