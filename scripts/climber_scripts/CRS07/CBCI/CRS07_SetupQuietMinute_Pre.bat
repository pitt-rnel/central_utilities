@echo off

:: Set subject
set SUBJECT_ID=CRS07
:: Set pedestal ID and cbInst ID params for 2 pedestals
set PED1=\
set CB1=0
set FIRST="quiet_minute"
set LAST="none"
set SUBFOLDER=QuietMinute_Pre

:: Enable raw
call ..\..\base_scripts\CRS_CentralAutoRaw.bat %CB1% 1

:: wait 2 seconds
@timeout /t 2 /nobreak

:: Fill in info
call ..\..\base_scripts\CRS_CentralAutoRecordInfo.bat %CB1% %SUBJECT_ID% %PED1% %FIRST% %LAST% %SUBFOLDER%

:: wait for input to disable raw data
@echo.
@echo "Press any key to disable raw data (after recordings are complete)"
@PAUSE

:: Disable raw
call ..\..\base_scripts\CRS_CentralAutoRaw.bat %CB1% 0