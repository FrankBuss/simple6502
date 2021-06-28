#include <stdio.h>
#include "application.h"
#include "hardware.h"
#include "mos6502.h"

static uint8_t busRead(uint16_t address);
static void busWrite(uint16_t address, uint8_t data);

uint64_t total_cycles;
mos6502 theCPU(busRead, busWrite);

// 64k memory
static uint8_t memory[1 << 16];

// 6522 ports
static uint8_t portA;
static uint8_t portB;

static bool running = false;
static bool step = false;

static uint8_t busRead(uint16_t address) {
    if (address >= 0xe000 && address <= 0xe0ff) {
        // 6522 emulation
        uint8_t reg = address - 0xe00;
        switch (reg) {
            case 0:
                return portB;
            case 1:
                return portA;
        }
        return 0;
    } else {
        // RAM/ROM emulation
        return memory[address];
    }
}

#define LCD_DB4_PIN 1
#define LCD_DB5_PIN 2
#define LCD_DB6_PIN 4
#define LCD_DB7_PIN 8
#define LCD_RS_PIN 16
#define LCD_E_PIN 32

extern void draw_display_text(int x0, int y0, char text);

static void busWrite(uint16_t address, uint8_t data) {
    static int lastE = 0;
    static int count = 0;
    static uint8_t byte = 0;
    if (address == 0xde00) {
        int nextE = (data & LCD_E_PIN) > 0;
        if (lastE == 1 && nextE == 0) {
            byte <<= 4;
            byte |= data & 0xf;
            count++;
            static int y = 0;
            static int x = 0;
            if (count == 2) {
                count = 0;
                int inst = (data & LCD_RS_PIN) == 0;
                if (inst) {
                    if (byte & 0x80) {
                        x = byte & 0x3f;
                        if (byte & 0x40) {
                            y = 1;
                        } else {
                            y = 0;
                        }
                    }
                } else {
                    draw_display_text(x, y, byte);
                }
            }
        }
        lastE = nextE;
    }
    if (address >= 0xe000 && address <= 0xe0ff) {
        // 6522 emulation
        uint8_t reg = address - 0xe00;
        switch (reg) {
            case 0:
                portB = data;
                break;
            case 1:
                portA = data;
                break;
        }
    } else {
        // RAM/ROM emulation, write only in RAM
        // TODO if (address < 0x8000) {
        if (address < 0xf000) {
            memory[address] = data;
        }
    }
}

// emulated frequency of the 6502
#define FREQUENCY 1000000

void start() {
    // init hardware
    set_red_led(true);

    // load EPROM
#if 1
    FILE* f = fopen("rom.bin", "rb");
    if (!f) {
        printf("can't open rom.bin\n");
        exit(1);
    }
    fread(&memory[1 << 15], 1 << 15, 1, f);
    fclose(f);
#else
    FILE* f = fopen("../../6502/lcd-test.prg", "rb");
    if (!f) {
        printf("can't open rom.bin\n");
        exit(1);
    }
    for (int i = 0; i < 65536; i++) {
        memory[i]=0x60;
    }
    fread(&memory[0x0801-2], 1 << 15, 1, f);
    fclose(f);
#endif

    // init CPU
    total_cycles = 0;
    theCPU.Reset();
    theCPU.pc = 2061;
}

void tick(uint32_t deltaMilliseconds) {
    // update CPU state
    static int cycles = 0;
    cycles += FREQUENCY * deltaMilliseconds / 1000;
    uint64_t cyclesUsed = 0;
    if (step) {
        theCPU.Run(1, cyclesUsed, mos6502::INST_COUNT);
        step = false;
    } else {
        if (running) {
            theCPU.Run(cycles, cyclesUsed, mos6502::CYCLE_COUNT);
        }
    }
    cycles -= cyclesUsed;
    total_cycles += cyclesUsed;

    // update emulated hardware
    set_red_led(portA & 1);
}

void onKeyDown(char key) {
    switch (key) {
        case ' ':
            step = true;
            break;
        case 'r':
            theCPU.Reset();
            break;
        case 'c':
            running = true;
            break;
        case 'b':
            running = false;
            break;
    }
}

void onKeyUp(char key) {

}
