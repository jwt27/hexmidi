#!/bin/bash
export PATH=$MSYS2_DJGPP/bin:$PATH;
unset CXXFLAGS
unset CFLAGS
make -Otarget -j`nproc --all` vs $@
