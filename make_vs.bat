cd /d %1
set MSYSTEM=MINGW64
set CHERE_INVOKING=1
%MSYS2_ROOT%/usr/bin/bash -lc "./make_vs.sh %2 %3 %4 %5 %6 %7 %8 %9"
