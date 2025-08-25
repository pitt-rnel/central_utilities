@echo off

:: Set cbInst ID params for 2 NSPs
set CB1=1
set CB2=2

:: Enable raw
call ..\base_scripts\CRS_CentralAutoRaw.bat %CB1% 0
call ..\base_scripts\CRS_CentralAutoRaw.bat %CB2% 0
