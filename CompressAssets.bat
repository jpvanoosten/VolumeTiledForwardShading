@echo off
pushd %~dp0

del /F /Q Assets.7z.*
.\Tools\7zip\7za.exe a -v500m Assets.7z Assets/models/*

popd

pause