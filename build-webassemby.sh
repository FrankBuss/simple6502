#!/bin/bash

source ~/emsdk/emsdk_env.sh

BuildDir="./embuild"          # root of output dir for built files
DistDir="./embuild/dist"      # distribution "binary" output dir

EM_CC="em++"
EM_CFLAGS="-s WASM=1 -O3"
EM_LDFLAGS=$"--bind -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS=[\"png\"] -s USE_SDL_TTF=2 --preload-file ./emulator/webassembly/background.png@background.png --preload-file ./emulator/webassembly/rom.bin@rom.bin --preload-file ./emulator/webassembly/Vera.ttf@Vera.ttf --use-preload-plugins"
echo $EM_LDFALGS

# start clean for this test
rm -fr "${BuildDir}"
mkdir -p "${BuildDir}"

# get all source files
srcFiles=""
for srcFile in ./{emulator/common,emulator/webassembly}/*.cpp; do
    srcFiles+="${srcFile} "
done
srcFiles+="./emulator/mos6502/mos6502.cpp "
srcFiles+="./emulator/dcc6502/dcc6502.cpp "

# compile 6502 assembler program
cd 6502
dasm rom.asm -f3 -orom.bin -llisting.txt
cd ..
cp ./6502/rom.bin ./emulator/webassembly

# compile and link object files into binary runtime in DistDir
mkdir -p "${DistDir}"
${EM_CC} -I ./emulator/common -I ./emulator/mos6502 ${srcFiles} ${EM_CFLAGS} ${EM_LDFLAGS} -o ${DistDir}/index.js -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]'
cp ./emulator/webassembly/index.html ${DistDir}
cp ./emulator/webassembly/Vera.ttf ${DistDir}
cp ./emulator/webassembly/background.png ${DistDir}
cp ./6502/rom.bin ${DistDir}

# insert current listing into index.html
sed '/%LISTING%/{
r 6502/listing.txt
d
}' ${DistDir}/index.html > ${DistDir}/index2.html
mv ${DistDir}/index2.html ${DistDir}/index.html

# expose to browser to test
cd "${DistDir}"
python3 -m http.server 8000
