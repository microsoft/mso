CALL "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" %BUILDPLATFORM% %BUILDWINDOWSSDK%
SET CC=%VCToolsInstallDir%\bin\Hostx64\%VSCMD_ARG_TGT_ARCH%\cl.exe
SET CXX=%VCToolsInstallDir%\bin\Hostx64\%VSCMD_ARG_TGT_ARCH%\cl.exe
ECHO CC=%CC%
ECHO CXX=%CXX%
