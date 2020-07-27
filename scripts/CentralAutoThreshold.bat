:: Script to threshold electrodes in Central
@echo off

:: Set Input Values
SET cbInst=0
SET threshold=-4.5
SET absFlag=0

:: bin dir
SET bin=..\bin

@echo on
::Run Central Auto Threshold
%bin%\CentralAutoThreshold.exe %cbInst% %threshold% %absFlag%