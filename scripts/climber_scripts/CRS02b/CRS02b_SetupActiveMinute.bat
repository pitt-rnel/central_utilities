@echo off

:: Set subject
set SUBJECT_ID=CRS02b
:: Set pedestal ID and cbInst ID params for 2 pedestals
set PED1=A
set PED2=P
set CB1=1
set CB2=2
set FIRST="active_minute"
set LAST="none"

:: Enable raw
call ..\base_scripts\CRS_CentralAutoRaw.bat %CB1% 1
call ..\base_scripts\CRS_CentralAutoRaw.bat %CB2% 1

:: wait 2 seconds
timeout /t 2 /nobreak

:: Fill in info
call ..\base_scripts\CRS_CentralAutoRecordInfo.bat %CB1% %SUBJECT_ID% %PED1% %FIRST% %LAST%
call ..\base_scripts\CRS_CentralAutoRecordInfo.bat %CB2% %SUBJECT_ID% %PED2% %FIRST% %LAST%

:: wait for input to disable raw data
echo "Press any key to disable raw data (after recordings are complete)"
PAUSE

:: Disable raw
call ..\base_scripts\CRS_CentralAutoRaw.bat %CB1% 0
call ..\base_scripts\CRS_CentralAutoRaw.bat %CB2% 0