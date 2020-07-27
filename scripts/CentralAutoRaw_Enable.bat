:: Script to enable raw data in Central
@echo off

:: Set Input Values
SET cbInst=0
SET enableFlag=1

:: bin dir
SET bin=..\bin

@echo on
::Run Central Auto Raw
%bin%\CentralAutoRaw.exe %cbInst% %enableFlag%