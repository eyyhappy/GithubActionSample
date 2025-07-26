@echo off

set curdir=%~dp0
cd /d %curdir%


REM ::批量将指定目录中的所有C文件用Astyle进行代码美化操作，支持鼠标拖动操作
REM ::参考http://blog.chinaunix.net/uid-23381466-id-58917.html
REM ::参考http://blog.csdn.net/windcao/article/details/12319225

REM ::设置astyle.exe执行路径
REM ::set astyle="%~dp0\astyle.exe"
set astyle="C:\Program Files\AStyle\bin\AStyle.exe" 

REM ::循环遍历目录查找指定文件进行处理
REM ::for /r %~dp1%~nx1 %%a in (.) do %astyle% %%a\*.cpp --style=ansi -s4 -S -N -p -L -N -m0 -M40 --convert-tabs --suffix=.pre %f
REM REM for /r %~dp1%~nx1 %%a in (.) do %astyle% %%a\*.c   --style=ansi -s4 -S -N -p -L -N -m0 -M40 --convert-tabs --delete-empty-lines --suffix=.pre %f
REM ::for /r %~dp1%~nx1 %%a in (.) do %astyle% %%a\*.hpp --style=linux -s4 -S -N -L -m0 -M40 --recursive --convert-tabs %f
REM REM for /r %~dp1%~nx1 %%a in (.) do %astyle% %%a\*.h   --style=ansi -s4 -S -N -p -L -N -m0 -M40 --convert-tabs --delete-empty-lines --suffix=.pre %f
REM ::for /r %~dp1%~nx1 %%a in (.) do %astyle% %%a\*.s   --style=linux -s4 -S -N -L -m0 -M40 --recursive --convert-tabs %f


for /r %~dp1%~nx1 %%a in (.) do %astyle% "%%a\*.c"   --style=ansi -s4 -S -N -p -L -N -m0 -M40 --convert-tabs --delete-empty-lines --indent-preprocessor --suffix=.orig %f
REM ::for /r %~dp1%~nx1 %%a in (.) do %astyle% %%a\*.hpp --style=linux -s4 -S -N -L -m0 -M40 --recursive --convert-tabs %f
for /r %~dp1%~nx1 %%a in (.) do %astyle% "%%a\*.h"   --style=ansi -s4 -S -N -p -L -N -m0 -M40 --convert-tabs --delete-empty-lines --indent-preprocessor --suffix=.orig %f
REM ::for /r %~dp1%~nx1 %%a in (.) do %astyle% %%a\*.s   --style=linux -s4 -S -N -L -m0 -M40 --recursive --convert-tabs %f



REM ::删除astyle生成的所有中间文件
for /r %~dp1%~nx1 %%a in (*.orig) do del %%a
for /r %~dp1%~nx1 %%a in (*.pre) do del %%a
for /r %~dp1%~nx1 %%a in (*.bak) do del %%a

pause


REM :: --style=ansi -s4 -S -N -R ./*.c ./*.h -p -L -N -m0 -M40 --convert-tabs --suffix=.pre %f