@echo off
pushd %~dp0

echo Initialize and update submodules (and submodules of submodules)...
git submodule update --init --recursive
echo Done initilizing submodules.

echo Building boost dependencies...
pushd .\externals\boost\
call .\bootstrap.bat
call .\b2 --with-serialization link=static runtime-link=shared threading=multi address-model=64 -j8 stage
echo Done building boost dependencies.
popd
popd

pause