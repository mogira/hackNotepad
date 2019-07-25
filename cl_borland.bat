@echo off
bcc32 -tW injector.c
bcc32 -eexDLL.dll -WD main.c other.c config.c back.c
