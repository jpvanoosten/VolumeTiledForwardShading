@echo off
pushd %~dp0

.\Tools\7zip\7za.exe a -v500m Assets.7z Assets/models/*

popd

pause