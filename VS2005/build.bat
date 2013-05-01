@echo off

echo Build the project!
echo Initializing VC2005 environment variables...
call "C:\Program Files\Microsoft Visual Studio 8\VC\vcvarsall.bat"
vcbuild "Torque SDK.sln" "Release|Win32"

IF ERRORLEVEL 1 EXIT 1

echo build installers!
cd  ..\install_build

echo    - build.sh win32 sdk
c:\cygwin\bin\sh -c "export PATH=/cygdrive/c/cygwin/bin:. && build.sh win32 sdk"
IF ERRORLEVEL 1 EXIT 1

echo    - copying output

mkdir %2\sdk
IF ERRORLEVEL 1 EXIT 1

move sdk\*.exe %2\sdk
IF ERRORLEVEL 1 EXIT 1

echo    - build.sh win32 demo

c:\cygwin\bin\sh -c "export PATH=/cygdrive/c/cygwin/bin:. && build.sh win32 demo"
IF ERRORLEVEL 1 EXITs 1

echo    - copying output

mkdir %2\demo
IF ERRORLEVEL 1 EXIT 1

move demo\*.exe %2\demo
IF ERRORLEVEL 1 EXIT 1

echo Coming home...
cd ..\vc7