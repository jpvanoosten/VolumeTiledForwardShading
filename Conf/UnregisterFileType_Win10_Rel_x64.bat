@echo off
REM Unregister a file handler for the .3dgep file types.
REM This is a configuration file format that is used to run
REM the GraphicsTest application.

pushd .

cd "%~dp0..\"

SET EXE_FILE=Game.exe
SET EXE_DIR=%CD%\bin\Release\
SET EXE_PATH="%EXE_DIR%%EXE_FILE%"
SET FILE_EXT=.3dgep
SET FILE_TYPE=text
SET PROG_ID=3DGEP.Launcher.dev

REG DELETE "HKCU\Software\Microsoft\Windows\CurrentVersion\App Paths\%EXE_FILE%" /f
REG DELETE "HKCU\Software\Classes\Applications\%EXE_FILE%" /f
REG DELETE "HKCU\Software\Classes\%FILE_EXT%" /f
REG DELETE "HKCU\Software\Classes\%PROG_ID%" /f
REG DELETE "HKCU\Software\Classes\SystemFileAssociations\%FILE_EXT%" /f

popd