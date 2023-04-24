:: Script to configure continuous stream in Central
:: First input is cbInst
:: Second input is rate index (0 is none, 1 is 0.5 kHz, 2 is 1 kHz, ... 5 is 30 kHz)
:: Third input is filter index (e.g. 6 is LP 250 Hz, 0 is none)
@echo off

:: Set Input Values
SET cbInst=%1
SET rateIndex=%2
SET filtIndex=%3

:: bin dir
SET bin=%~dp0\..\..\..\bin

@echo on
::Run Central Auto Continuous
%bin%\CentralAutoContinuous.exe %cbInst% %rateIndex% %filtIndex%