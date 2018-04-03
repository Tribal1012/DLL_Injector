@echo off
:Start
SET VSPATH=C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC

SET COMSPEC=cmd.exe

:BuildCommand
ECHO "================================================="
ECHO "| [+] Environment variables are ready to build. |"
ECHO "| [+] Please enter the following command:       |"
ECHO "|                                               |"
ECHO "|     nmake -f dll_injector.mk all              |"
ECHO "================================================="

:VScmd
%comspec% /k ""%VSPATH%\vcvarsall.bat"" x86

