@echo off

:: 引数のチェック
if "%~1"=="" (
    echo Error: Source file not specified.
    pause
    exit /b 1
)
set copyfrom=%~1
set "encode_copyfrom=%copyfrom: =___DUMMYSPACE___%"
set "decode_copyfrom=%encode_copyfrom:___DUMMYSPACE___= %"

if "%~2"=="" (
    echo Error: Target file not specified.
    pause
    exit /b 1
)
set copyto=%~2
set "encode_copyto=%copyto: =___DUMMYSPACE___%"
set "decode_copyto=%encode_copyto:___DUMMYSPACE___= %"

:: 管理者権限チェック
net session >nul 2>&1
if %errorlevel% neq 0 (
    :: 管理者権限で再実行
    powershell -Command "Start-Process '%~f0' -Verb RunAs -ArgumentList %encode_copyfrom%, %encode_copyto%"
    exit /b
)

:: コピー実行
copy "%decode_copyfrom%" "%decode_copyto%"
if %errorlevel% equ 0 (
    echo Success: Copied "%decode_copyfrom%" to "%decode_copyto%".
) else (
    echo Error: Copy failed. Error: %errorlevel%
    pause
    exit /b %errorlevel%
)

