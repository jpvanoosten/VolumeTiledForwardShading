@ECHO OFF
PUSHD %~dp0

DEL /F /Q Assets.7z.*
.\Tools\7zip\7za.exe a -v500m Assets.7z Assets/models/*

POPD

PAUSE