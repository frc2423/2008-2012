@echo off

set ec=1

set PYTHON_EXE=python.exe

set PYTHON_BIN=C:\Python33\%PYTHON_EXE%
if exist %PYTHON_BIN% goto python_found

set PYTHON_BIN=C:\Python32\%PYTHON_EXE%
if exist %PYTHON_BIN% goto python_found

echo Error: Could not find python 3
exit /b 1

:python_found
%PYTHON_BIN% -B "%~dp0\test.py" %*

if "%errorlevel%" == "0" set ec=0
pause

exit /b %ec%
