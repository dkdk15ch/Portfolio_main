@echo off
chcp 65001 > nul
cd /d %~dp0
echo [Archiver] 焼き上げ中...
project\x64\Release\Archiver.exe project\x64\Release\HLSL
echo [Archiver] 完了！
pause