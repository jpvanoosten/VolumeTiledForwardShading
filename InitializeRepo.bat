@ECHO OFF
PUSHD %~dp0

ECHO Initialize and update submodules (and submodules of submodules)...
git submodule update --init --recursive
ECHO Done initilizing submodules.

PUSHD .\externals\boost\
CALL .\bootstrap.bat
CALL .\b2 headers
CALL .\b2 --with-serialization --stagedir=./ link=static runtime-link=shared threading=multi address-model=64 -j8 stage
POPD
POPD

PAUSE