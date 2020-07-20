:: Script to run Central recording
@echo off

:: Set Input Values
SET cbInst=0
SET subjectID=TEST_SUBJECT
SET duration=30
SET datapath="C:\data\BlackrockData\Baseline"

:: bin dir
SET bin=..\bin

@echo on
::Run Central Auto Record
%bin%\CentralAutoRecord.exe %cbInst% %subjectID% %duration% %datapath%