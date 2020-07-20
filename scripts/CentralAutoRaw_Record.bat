:: Central script to enable raw data, record, and then disable raw data.
@echo off

call CentralAutoSettings_Save.bat
call CentralAutoRaw_Enable.bat
timeout /t 2 /nobreak
call CentralAutoRecord.bat
call CentralAutoRaw_Disable.bat
