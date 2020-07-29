:: Script to threshold electrodes in Central
:: First input is cbInst
:: Second input is RMS threshold
@echo off

:: Set Input Values
SET cbInst=%1
SET threshold=%2
SET absFlag=0

:: bin dir
SET bin=%~dp0\..\..\..\bin

@echo on
::Run Central Auto Threshold
%bin%\CentralAutoThreshold.exe %cbInst% %threshold% %absFlag%