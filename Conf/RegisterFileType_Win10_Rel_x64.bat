@echo off
REM Register a file handler for the .3dgep file types.
REM This is a configuration file format that is used to run
REM the GraphicsTest application.

pushd .

cd "%~dp0..\"

SET EXE_FILE=Game.exe
SET EXE_DIR=%CD%\bin\Release
SET EXE_PATH="%EXE_DIR%%EXE_FILE%"
SET FILE_EXT=.3dgep
SET FILE_TYPE=text
SET PROG_ID=3DGEP.Launcher.dev

REG ADD "HKCU\Software\Microsoft\Windows\CurrentVersion\App Paths\%EXE_FILE%" /ve /t REG_SZ /d %EXE_PATH% /f
REG ADD "HKCU\Software\Microsoft\Windows\CurrentVersion\App Paths\%EXE_FILE%" /v Path /t REG_SZ /d %EXE_DIR% /f
REG ADD "HKCU\Software\Classes\Applications\%EXE_FILE%" /v "FriendlyAppName" /t REG_SZ /d "3D Game Engine Programming" /f
REG ADD "HKCU\Software\Classes\Applications\%EXE_FILE%\DefaultIcon" /ve /t REG_EXPAND_SZ /d %EXE_PATH%,0 /f
REG ADD "HKCU\Software\Classes\Applications\%EXE_FILE%\shell\Open\command" /ve /t REG_SZ /d """%EXE_PATH%"" -c ""%%1""" /f
REG ADD "HKCU\Software\Classes\Applications\%EXE_FILE%\SupportedTypes" /v %FILE_EXT% /t REG_SZ /f
REG ADD "HKCU\Software\Classes\%FILE_EXT%" /v PerceivedType /t REG_SZ /d %FILE_TYPE% /f
REG ADD "HKCU\Software\Classes\%FILE_EXT%" /ve /t REG_SZ /d %PROG_ID% /f
REG ADD "HKCU\Software\Classes\%FILE_EXT%\OpenWithProgIds" /v %PROG_ID% /t REG_NONE /f
REG ADD "HKCU\Software\Classes\%PROG_ID%\DefaultIcon" /ve /t REG_EXPAND_SZ /d %EXE_PATH%,0 /f
REG ADD "HKCU\Software\Classes\%PROG_ID%\shell\Open\command" /ve /t REG_SZ /d """%EXE_PATH%"" -c ""%%1""" /f
REG ADD "HKCU\Software\Classes\SystemFileAssociations\%FILE_EXT%" /v PerceivedType /t REG_SZ /d %FILE_TYPE% /f

popd