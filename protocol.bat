@echo on
pushd %~dp0
del /F /Q Network\*_generated.h
FOR /F "tokens=1" %%a IN ('dir /B "Protocol" ^| findstr .fbs') DO ( 
	thirdparty\flatbuffers\bin\flatc.exe --cpp --cpp-std c++17 -o Network Protocol\%%a
)
echo #pragma once > Network\Protocol.h 
FOR /F "tokens=1" %%a IN ('dir /B "Network\*_generated.h"') DO ( 
	echo #include "%%a" >> Network\Protocol.h 
)
popd %~dp0

pause