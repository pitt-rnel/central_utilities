:: Script to enable raw data in Central
:: First input is cbInst
:: Second input is enableFlag (0 to disable raw, 1 to enable raw)
@echo off

:: Set Input Values
SET cbInst=0
SET enableFlag=1

:: bin dir
SET bin=%~dp0\..\..\..\..\bin

@echo on
::Run Central Auto Raw
%bin%\CentralAutoRaw.exe %cbInst% %enableFlag%