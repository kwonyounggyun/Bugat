@echo off
pushd %~dp0
rmdir /S /Q Network\Packet\Headers
mkdir Network\Packet\Headers
FOR /F "tokens=1" %%a IN ('dir /B "Network\Packet\fbs" ^| findstr .fbs') DO ( 
	thirdparty\flatbuffers\bin\flatc.exe --cpp --cpp-std c++17 -o Network\Packet\Headers Network\Packet\fbs\%%a
)
echo #pragma once > Network\Protocol.h 
FOR /F "tokens=1" %%a IN ('dir /B "Network\Packet\Headers" ^| findstr .h') DO ( 
	echo #include "Packet/Headers/%%a" >> Network\Protocol.h 
)
popd %~dp0

pause