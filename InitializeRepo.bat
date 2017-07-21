@echo off
pushd %~dp0

REM Initialize and update submodules (and submodules of submodules)
REM git submodule update --init --recursive

REM Bootstrap boost and gernate headers
pushd .\externals\boost\
echo Boost bootstrap...
call .\bootstrap.bat
echo Finished bootstrap.
echo Generating Boost headers...
call .\b2 toolset=msvc-14.1 headers
echo Finished generating Boost headers.
popd
popd

pause