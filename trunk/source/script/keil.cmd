pushd %~dp0
path=%path%;%cd%
call ini2inc.armcc.cmd %cd%\..\platform\board\%1\%2\fairq.ini %cd%\..\include\%1\%2\armcc\fairq.inc
call ini2enum.armcc.cmd %cd%\..\platform\board\%1\%2\fairq.ini %cd%\..\include\%1\%2\fairq.h
