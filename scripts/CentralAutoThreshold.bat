:: Script to run Central recording
@echo off

:: Set Input Values
SET cbInst=0
SET threshold=-4.5
SET absFlag=0

:: bin dir
SET bin=..\bin

@echo on
::Run Central Auto Settings
%bin%\CentralAutoThreshold.exe %cbInst% %threshold% %absFlag%