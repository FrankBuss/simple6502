; compile command line: dasm rom.asm -f3 -orom.bin -llisting.txt
; DASM: https://dasm-assembler.github.io/

		processor 6502

LCD_DB4_PIN equ 1
LCD_DB5_PIN equ 2
LCD_DB6_PIN equ 4
LCD_DB7_PIN equ 8
LCD_RS_PIN equ 16
LCD_E_PIN equ 32

LCD_PORT equ $e001

latch 	EQU $fb
text	EQU $fc
tmp1	EQU $fe
tmp2	EQU $be
textX	EQU $bf

		org $8000

		; init stack pointer and clear decimal flag
reset	sei
		ldx #$ff
		txs
		cld

		; set all port pins to output in 6522, and low
		stx $e002
		stx $e003
		lda #0
		sta $e000
		sta $e001

		; init display
		jsr lcdInit

		; scroll text
start   lda #0
		sta textX
start2	; draw first line
		clc
		lda #<(scroll1)
		adc textX
		sta text
		lda #>(scroll1)
		sta text + 1
		ldy #0
		jsr drawTextLine

		; draw second line
		clc
		lda #<(scroll2)
		adc textX
		sta text
		lda #>(scroll2)
		sta text + 1
		ldy #1
		jsr drawTextLine

		; wait a bit
		lda #100
		jsr delay

		; scroll
		inc textX
		lda textX
		cmp #56
		bne start2

		; start from beginning
		jmp start

		; blink LED at PA0
blink	lda #0
		sta $e001
		jsr delay
		lda #1
		sta $e001
		jsr delay
		jmp blink

		; delay for the specified numbers of milliseconds in A
delay	ldx #2
delay2	ldy #0
delay3	dey
		bne delay3
		dex
		bne delay2
		sec
		sbc #1
		cmp #0
		bne delay
		rts

		; set pin to 1 or 0 with latch
		; A: pin mask
		; X: 1: set port pin to 1, 0: set port pin to 0
digitalWrite
		cpx #0
		bne digitalWriteSet
		eor #$ff
		and latch
		sta latch
		jmp digitalWriteEnd
digitalWriteSet
		ora latch
		sta latch
digitalWriteEnd
		sta LCD_PORT
		rts

		; send nibble in A to display
lcdSendNibble
		pha
		lda latch
		and #$f0
		sta latch
		pla
		ora latch
		sta latch

		lda #LCD_E_PIN
		ldx #1
		jsr digitalWrite

		lda #LCD_E_PIN
		ldx #0
		jmp digitalWrite

		; send data as instruction or data to display
		; A: data to send
		; X: state of RS pin: 0 for instruction, 1 for data
lcdSend	pha
		lda #LCD_RS_PIN
		jsr digitalWrite
		pla
		pha
		lsr
		lsr
		lsr
		lsr
		jsr lcdSendNibble
		pla
		and #$f
		jmp lcdSendNibble

		; send instruction to display
		; A: instruction to send
lcdSendInstruction
		ldx #0
		jmp lcdSend

		; send data to display
		; A: data to send
lcdSendData
		ldx #1
		jmp lcdSend

		; init LCD
lcdInit
		; init 4 bit mode
		lda #LCD_RS_PIN
		ldx #0
		jsr digitalWrite
		lda #LCD_E_PIN
		ldx #0
		jsr digitalWrite

		lda #15
		jsr delay

		lda #3
		jsr lcdSendNibble
		lda #5
		jsr delay

		lda #3
		jsr lcdSendNibble
		lda #1
		jsr delay

		lda #3
		jsr lcdSendNibble
		lda #5
		jsr delay

		lda #2
		jsr lcdSendNibble

  		; 2 line mode, 5x11 characters
		lda #5
		jsr delay
		lda #$28
  		jsr lcdSendInstruction

  		; display off, cursor off, blink off
		lda #5
		jsr delay
		lda #8
  		jsr lcdSendInstruction

  		; clear display and return cursor home
		lda #5
		jsr delay
		lda #1
  		jsr lcdSendInstruction

		; entry mode: left to right
		lda #5
		jsr delay
		lda #6
  		jsr lcdSendInstruction

  		; display on
		lda #5
		jsr delay
		lda #$c
  		jmp lcdSendInstruction

		; set position in X/Y
setPosition
		txa
		cpy #1
		bne setPosition2
		ora #$40
setPosition2		
		ora #$80
		jmp lcdSendInstruction

		; set char in A at position X/Y
setChar
		pha
		jsr setPosition
		pla
		jmp lcdSendData

		; draw 16 characters starting from "text" at line in Y
drawTextLine
		lda #0
		sta tmp1
		sty tmp2
drawTextLine2
		ldy tmp1
		lda (text),y
		ldx tmp1
		ldy tmp2
		jsr setChar
		inc tmp1
		lda tmp1
		cmp #16
		bne drawTextLine2
		rts

scroll1 dc "                  Hello               Have a nice day                    ";
scroll2 dc "                   Bil                 Cheers Frank                      ";

nmi		rti

irq		rti

		org $fffa
		; NMI vector
		dc <(nmi), >(nmi)
		; reset vector
		dc <(reset), >(reset)
		; IRQ/BRK vector
		dc <(irq), >(irq)
end
