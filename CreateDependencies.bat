@echo off
pushd %~dp0

.\Tools\7zip\7za.exe a -v500m Dependencies.7z Game/Assets/models/*

popd

pause