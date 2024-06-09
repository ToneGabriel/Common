@echo off
setlocal enabledelayedexpansion

:: Get the number of logical processors
for /f "tokens=2 delims==," %%a in ('wmic cpu get NumberOfLogicalProcessors /value') do (
    set "NUM_PROCESSORS=%%a"
    goto :next
)

:next

:: Get the number of cores
for /f "tokens=2 delims==," %%a in ('wmic cpu get NumberOfCores /value') do (
    set "NUM_CORES=%%a"
    goto :done
)

:done
echo %NUM_PROCESSORS%
echo %NUM_CORES%
