@ECHO OFF
PUSHD %~dp0

ECHO Initialize and update submodules...
git submodule update --init --recursive
ECHO Done initilizing submodules.
POPD

PAUSE