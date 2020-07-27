:: Script to load Central settings
@echo off

:: Set Input Values
SET filename="C:\ccf_settings\test_settings.ccf"
SET cbInst=0
SET writeFlag=0

:: bin dir
SET bin=..\bin

@echo on
::Run Central Auto Settings
%bin%\CentralAutoSettings.exe %filename% %cbInst% %writeFlag%