#!/bin/bash

BuildDir="./linuxbuild"          # root of output dir for built files
DistDir="./linuxbuild/dist"      # distribution "binary" output dir

CC="g++"
CFLAGS="-Wall -O3"
LDFLAGS="-lSDL2 -lSDL2_ttf -lSDL2_image"

# start clean for this test
rm -fr "${BuildDir}"
mkdir -p "${BuildDir}"

# get all source files
srcFiles=""
for srcFile in ./{emulator/common,emulator/webassembly}/*.cpp; do
    srcFiles+="${srcFile} "
done
srcFiles+="./emulator/mos6502/mos6502.cpp "

# compile 6502 assembler program
cd 6502
dasm rom.asm -f3 -orom.bin -llisting.txt
cd ..

# compile and link object files into binary runtime in DistDir
mkdir -p "${DistDir}"
${CC} -I ./emulator/common -I ./emulator/mos6502 ${srcFiles} ${CFLAGS} ${LDFLAGS} -o ${DistDir}/simple6502
cp ./emulator/webassembly/index.html ${DistDir}
cp ./emulator/webassembly/Vera.ttf ${DistDir}
cp ./emulator/webassembly/background.png ${DistDir}
cp ./6502/rom.bin ${DistDir}

# start program
cd "${DistDir}"
./simple6502
