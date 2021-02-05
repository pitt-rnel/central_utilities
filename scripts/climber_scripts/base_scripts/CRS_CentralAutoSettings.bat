:: Script to save Central Settings
:: First input is cbInst
:: Second input is writeFlag (0 load last, 1 save, -1 load template)
:: Third input is subject id
:: Fourth input is pedestal ID (e.g. A or P)
:: Fifth input is optional file suffix (e.g. _cereE or _stim)
@echo off
setlocal ENABLEDELAYEDEXPANSION

:: Set Input Values
SET cbInst=%1
SET writeFlag=%2
SET SUBJECT_ID=%3
SET PEDESTAL=%4
SET SUFFIX=%5

SET BR_DATA_DIR=D:\git\climber\data\BlackrockData
SET EXT=.ccf
SET SUBJ_DATA_DIR=%BR_DATA_DIR%\%SUBJECT_ID%\CentralSettings

IF %writeFlag% == 0 (
    :: load last
    for /f %%i in ('dir /b/a-d/od/t:c %SUBJ_DATA_DIR%\*_%PEDESTAL%%SUFFIX%*%EXT%') do set LASTCCF=%%i
    ::echo LASTCCF=!LASTCCF!
    SET filename="%SUBJ_DATA_DIR%\!LASTCCF!"
    echo reading !filename!
)
IF  %writeFlag% == 1 (
    :: write file with generated datestamp
    for /f "tokens=2 delims==" %%a in ('wmic OS Get localdatetime /value') do set "dt=%%a"
    set "YY=!dt:~2,2!" & set "YYYY=!dt:~0,4!" & set "MM=!dt:~4,2!" & set "DD=!dt:~6,2!"
    set datestamp=!YYYY!_!MM!_!DD!

    :: generate filename
    SET filename="!SUBJ_DATA_DIR!\!datestamp!_!PEDESTAL!!SUFFIX!!EXT!"
    echo Saving !filename!
)
IF %writeFlag% == -1 (
    :: load template
    SET filename="!SUBJ_DATA_DIR!\Template_!PEDESTAL!!SUFFIX!!EXT!"
    echo reading !filename!
    SET writeFlag=0
)

:: bin dir
SET bin=%~dp0\..\..\..\bin

@echo on
::Run Central Auto Settings
%bin%\CentralAutoSettings.exe %filename% %cbInst% %writeFlag%