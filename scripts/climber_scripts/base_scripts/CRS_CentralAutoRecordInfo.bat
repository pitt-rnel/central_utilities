:: Script to run Central recording
:: First input is cbInst
:: Second input is subject id
:: Third input is pedestal ID (e.g. A or P)
:: Forth input is "First Name"
:: Fifth Input is "Last Name"
@echo off

:: Set Input Values
SET cbInst=%1
SET SUBJECT_ID=%2
SET PEDESTAL=%3
SET FIRST=%4
SET LAST=%5

:: duration = -2 means fill in info w/out recording. Set first/last names to none
SET DURATION=-2

SET BR_DATA_DIR=C:\git\climber\data\BlackrockData

:: generate datestamp, will set DOB to current day
for /f "tokens=2 delims==" %%a in ('wmic OS Get localdatetime /value') do set "dt=%%a"
set "YY=%dt:~2,2%" & set "YYYY=%dt:~0,4%" & set "MM=%dt:~4,2%" & set "DD=%dt:~6,2%"

:: generate filename
SET DATAPATH="%BR_DATA_DIR%\%SUBJECT_ID%\Baseline%PEDESTAL%"

:: bin dir
SET bin=%~dp0\..\..\..\bin

@echo on
::Run Central Auto Record
%bin%\CentralAutoRecord.exe %cbInst% %SUBJECT_ID% %DURATION% %DATAPATH% %FIRST% %LAST% %MM% %DD% %YYYY%