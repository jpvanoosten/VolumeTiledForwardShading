@ECHO OFF

REM ****************************************************************************
REM MIT License
REM 
REM Copyright (c) 2018 Jeremiah van Oosten
REM 
REM Permission is hereby granted, free of charge, to any person obtaining a copy
REM of this software and associated documentation files (the "Software"), to deal
REM in the Software without restriction, including without limitation the rights
REM to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
REM copies of the Software, and to permit persons to whom the Software is
REM furnished to do so, subject to the following conditions:
REM 
REM The above copyright notice and this permission notice shall be included in all
REM copies or substantial portions of the Software.
REM 
REM THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
REM IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
REM FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
REM AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
REM LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
REM OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
REM SOFTWARE.
REM ****************************************************************************

REM This script will generate the solution files for the currently installed version
REM of Visual Studio detected on the user's computer. Supported version of Visual Studio are:
REM     Visual Studio 2019
REM     Visual Studio 2017
REM     Visual Studio 2015
REM 
REM The solution files will be created in Build_vs2017 if Visual Studio 2017 was detected or
REM in Build_vs2015 if Visual Studio 2015 was detected.

PUSHD %~dp0

REM Update these lines if the currently installed version of Visual Studio is not 2017.
SET VSWHERE="%~dp0\Tools\vswhere\vswhere.exe"
SET CMAKE="%~dp0\Tools\cmake-3.14.0-win64-x64\bin\cmake.exe"

REM Detect latest version of Visual Studio.
FOR /F "usebackq delims=." %%i IN (`%VSWHERE% -latest -prerelease -requires Microsoft.VisualStudio.Workload.NativeGame -property installationVersion`) DO (
    SET VS_VERSION=%%i
)

IF %VS_VERSION% == 16 (
    SET CMAKE_GENERATOR="Visual Studio 16 2019"
    SET CMAKE_BINARY_DIR=build_vs2019
) ELSE IF %VS_VERSION% == 15 (
    SET CMAKE_GENERATOR="Visual Studio 15 2017"
    SET CMAKE_BINARY_DIR=build_vs2017
) ELSE IF %VS_VERSION% == 14 (
    SET CMAKE_GENERATOR="Visual Studio 14 2015"
    SET CMAKE_BINARY_DIR=build_vs2015
) ELSE (
    ECHO.
    ECHO ***********************************************************************
    ECHO *                                                                     *
    ECHO *                                ERROR                                *
    ECHO *                                                                     *
    ECHO ***********************************************************************
    ECHO No compatible version of Microsoft Visual Studio detected.
    ECHO Please make sure you have Visual Studio 2015 ^(or newer^) and the 
    ECHO "Game Development with C++" workload installed before running this script.
    ECHO Go to https://www.visualstudio.com/downloads/ to download the latest version of Visual Studio.
    ECHO. 
    PAUSE
    GOTO :Exit
)

:GenerateProjectFiles
MKDIR %CMAKE_BINARY_DIR% 2>NUL
PUSHD %CMAKE_BINARY_DIR%
%CMAKE% -G %CMAKE_GENERATOR% -A x64 -Wno-dev "%~dp0"

IF %ERRORLEVEL% NEQ 0 (
    PAUSE
) ELSE (
    START VolumeTiledForwardShading.sln
)

POPD

:Exit

POPD
