# simple6502
A simple computer emulator with a 6502 CPU.

# build instructions for standalone Linux

First install SDL2, SDL2_ttf and SDL2_image. For example in Debian:

```
sudo apt-get install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev
```

Then install DASM from [here](https://dasm-assembler.github.io/) for compiling 6502 assembler programs. It must be in the path for the build scripts.

Then you can execute the script `build-linux.sh`, which will compile the emulator and start it.

# build instructions for WebAssembly version

First install emscripten, as explained [here](https://emscripten.org/docs/getting_started/downloads.html). Then you can execute the script `build-webassembly.sh`, which will compile the project for WebAssembly, and which starts a local webserver at localhost:8000 for testing.

# headless mode

If the image file `background.png` is not in the current directory, then no window is displayed. This is useful for headless mode when you just want to execute a 6502 assembler program.

There are 2 special addresses for headless mode:

0xffff: If you store a byte to this address, then it is printed on stdout (unbuffered).
0xfffe: Storing a byte to this address exits the emulator, with the byte stored as the exit code.

See the example program in hello.asm in the 6502 directory how to use it. After compiling the Linux version, you can compile the assembler program like this:

```
dasm 6502/hello.asm -f3 -ohello.bin
```

This creates the file `hello.bin`, which is the compiled output. You can specify this as a commandline argument to the emulator to start it:

```
linuxbuild/dist/simple6502 hello.bin
```

The result should be a line `Hello World!`.
