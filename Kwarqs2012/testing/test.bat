@echo off

set ec=1

c:\Python32\python.exe -B "%~dp0\test.py" %*

if "%errorlevel%" == "0" set ec=0
pause

exit /b %ec%
