@ECHO off
pushd %~dp0

set OutDir=%1
set TargetName=%2
set Platform=%3

IF NOT EXIST .\Dist\%Platform% (
	mkdir .\Dist\%Platform%
)

xcopy %OutDir%%TargetName%.dll .\Dist\%Platform% /D /Y /J /Q
xcopy %OutDir%%TargetName%.lib .\Dist\%Platform% /D /Y /J /Q
xcopy Source\FreeImage.h .\Dist\%Platform% /D /Y /Q
xcopy .\Dist\%Platform%\*.dll ..\..\Game\bin /D /Y /J /Q

:Exit

popd
