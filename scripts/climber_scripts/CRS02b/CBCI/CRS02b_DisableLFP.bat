:: Script to enable LFP data in Central
@echo off

:: Set Input Values
SET cbInst=0
SET rateIndex=0
SET filtIndex=0

:: bin dir
SET bin=%~dp0\..\..\..\..\bin

@echo on
::Run Central Auto Continuous
%bin%\CentralAutoContinuous.exe %cbInst% %rateIndex% %filtIndex%