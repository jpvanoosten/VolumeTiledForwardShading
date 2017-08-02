@ECHO OFF

REM ****************************************************************************
REM MIT License
REM 
REM Copyright (c) 2017 Jeremiah van Oosten
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
REM
REM This script tries to detect wich version of Visual Studio is currenlty installed
REM and returns the version in the VS_VERSION environment variable. If no version 
REM of Visual Studio was found, this script returns 1 and the VS_VERSION environment
REM variable remains empty.

SET VS_VERSION=

FOR /F "tokens=2,*" %%i IN ('REG QUERY HKLM\SOFTWARE\Microsoft\DevDiv\vs\Servicing\15.0\Core /v Install 2^>NUL') DO (
	IF %%j==0x1 (
		SET VS_VERSION=15.0
		GOTO Exit
	)
)

FOR /F "tokens=2,*" %%i IN ('REG QUERY HKLM\SOFTWARE\WOW6432Node\Microsoft\DevDiv\vs\Servicing\15.0\Core /v Install 2^>NUL') DO (
	IF %%j==0x1 (
		SET VS_VERSION=15.0
		GOTO Exit
	)
)

FOR /F "tokens=2,*" %%i IN ('REG QUERY HKLM\SOFTWARE\Microsoft\DevDiv\vs\Servicing\14.0\community /v Install 2^>NUL') DO (
	IF %%j==0x1 (
		SET VS_VERSION=14.0
		GOTO Exit
	)
)

FOR /F "tokens=2,*" %%i IN ('REG QUERY HKLM\SOFTWARE\WOW6432Node\Microsoft\DevDiv\vs\Servicing\14.0\community /v Install 2^>NUL') DO (
	IF %%j==0x1 (
		SET VS_VERSION=14.0
		GOTO Exit
	)
)

ECHO An installation of Visual Studio was not found on this computer.
EXIT /B 1

:Exit

ECHO Visual Studio %VS_VERSION% was found on this system.
EXIT /B 0