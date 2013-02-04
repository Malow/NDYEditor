@ECHO OFF

:: Copy Includes
IF EXIST External\NDYGFX\Include rmdir /q /s External\NDYGFX\Include
IF NOT EXIST External\NDYGFX\Include mkdir External\NDYGFX\Include
xcopy /s /e ..\NDYGFX\NDYGFX\Include External\NDYGFX\Include

:: Copy Lib
IF EXIST External\NDYGFX\Lib\NDYGFXD.lib del External\NDYGFX\Lib\NDYGFXD.lib
IF EXIST External\NDYGFX\Lib\NDYGFX.lib del External\NDYGFX\Lib\NDYGFX.lib
IF NOT EXIST External\NDYGFX\Lib\ mkdir External\NDYGFX\Lib
IF EXIST ..\NDYGFX\Build\Debug\NDYGFXD.lib copy ..\NDYGFX\Build\Debug\NDYGFXD.lib External\NDYGFX\Lib\NDYGFXD.lib
IF EXIST ..\NDYGFX\Build\Release\NDYGFX.lib copy ..\NDYGFX\Build\Release\NDYGFX.lib External\NDYGFX\Lib\NDYGFX.lib

:: Copy DLL
IF EXIST Bin\Debug\BTHFBXD.dll del Bin\Debug\BTHFBXD.dll
IF EXIST Bin\Release\BTHFBX.dll del Bin\Release\BTHFBX.dll
IF EXIST Bin\Debug\NDYGFXD.dll del Bin\Debug\NDYGFXD.dll
IF EXIST Bin\Release\NDYGFX.dll del Bin\Release\NDYGFX.dll
IF NOT EXIST Bin\Debug mkdir Bin\Debug
IF NOT EXIST Bin\Release mkdir Bin\Release
IF EXIST ..\NDYGFX\Build\Debug\BTHFBXD.dll copy ..\NDYGFX\Build\Debug\BTHFBXD.dll Bin\Debug\BTHFBXD.dll
IF EXIST ..\NDYGFX\Build\Release\BTHFBX.dll copy ..\NDYGFX\Build\Release\BTHFBX.dll Bin\Release\BTHFBX.dll
IF EXIST ..\NDYGFX\Build\Debug\NDYGFXD.dll copy ..\NDYGFX\Build\Debug\NDYGFXD.dll Bin\Debug\NDYGFXD.dll
IF EXIST ..\NDYGFX\Build\Release\NDYGFX.dll copy ..\NDYGFX\Build\Release\NDYGFX.dll Bin\Release\NDYGFX.dll

:: Copy Shaders
IF EXIST WorkingDir\Shaders\ rmdir /q /s WorkingDir\Shaders
IF NOT EXIST WorkingDir\Shaders\ mkdir WorkingDir\Shaders
xcopy /s /e ..\NDYGFX\WorkingDir\Shaders WorkingDir\Shaders


ECHO All done!
pause