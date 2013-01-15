@ECHO OFF

:: Copy Includes
IF EXIST External\NDYGFX\Include rmdir /q /s External\NDYGFX\Include
IF NOT EXIST External\NDYGFX\Include mkdir External\NDYGFX\Include
xcopy /s /e ..\NDYGFX\Include External\NDYGFX\Include

:: Copy Lib
IF EXIST External\NDYGFX\Lib\NDYGFXD.lib del External\NDYGFX\Lib\NDYGFXD.lib
IF EXIST External\NDYGFX\Lib\NDYGFX.lib del External\NDYGFX\Lib\NDYGFX.lib
IF NOT EXIST External\NDYGFX\Lib\ mkdir External\NDYGFX\Lib
IF EXIST ..\NDYGFX\Build\Debug\NDYGFXD.lib copy ..\NDYGFX\Build\Debug\NDYGFXD.lib External\NDYGFX\Lib\NDYGFXD.lib
IF EXIST ..\NDYGFX\Build\Release\NDYGFX.lib copy ..\NDYGFX\Build\Release\NDYGFX.lib External\NDYGFX\Lib\NDYGFX.lib

:: Copy DLL
IF EXIST Bin\Debug\NDYGFXD.dll del Bin\Debug\NDYGFXD.dll
IF EXIST Bin\Release\NDYGFX.dll del Bin\Release\NDYGFX.dll
IF NOT EXIST C:\notdeadyet\editor\Bin\Debug mkdir C:\notdeadyet\editor\Bin\Debug
IF NOT EXIST C:\notdeadyet\editor\Bin\Release mkdir C:\notdeadyet\editor\Bin\Release
IF EXIST ..\NDYGFX\Build\Debug\NDYGFXD.dll copy ..\NDYGFX\Build\Debug\NDYGFXD.dll Bin\Debug\NDYGFXD.dll
IF EXIST ..\NDYGFX\Build\Release\NDYGFX.dll copy ..\NDYGFX\Build\Release\NDYGFX.dll Bin\Release\NDYGFX.dll

:: Copy Shaders
IF EXIST WorkingDir\Shaders\ rmdir /q /s WorkingDir\Shaders
IF NOT EXIST WorkingDir\Shaders\ mkdir WorkingDir\Shaders
xcopy /s /e ..\NDYGFX\WorkingDir\Shaders WorkingDir\Shaders


ECHO All done!
pause