:: Save datestamped CCF Settings File
@echo off

:: Set subject and suffix parameters
set SUBJECT_ID=CRS07
set SUFFIX=_cereE
:: Set pedestal ID and cbInst ID params for 2 pedestals
set PED1=256
set CB1=0

:: Toggle load (0) or save (1) or load template (-1)
set SAVE_FLAG=1

:: First pedestal
call ..\..\base_scripts\CRS_CentralAutoSettings.bat %CB1% %SAVE_FLAG% %SUBJECT_ID% %PED1% %SUFFIX%