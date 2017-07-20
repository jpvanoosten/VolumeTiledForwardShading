@echo off
pushd %~dp0

xcopy ..\bin\*.dll ..\..\Game\bin /D /Y /J
xcopy ..\bin\*.pdb ..\..\Game\bin /D /Y /J

:Exit

popd
