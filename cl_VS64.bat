@echo off
cl /D WIN32 injector.c
cl /Feexdll.dll /LD main.c other.c config.c back.c
