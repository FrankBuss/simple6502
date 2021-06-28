; compile command line: dasm hello.asm -f3 -ohello.bin -llisting.txt
; DASM: https://dasm-assembler.github.io/

		processor 6502

textOut equ 0xffff
exit	equ 0xfffe

		org 0x8000

		; init stack pointer and clear decimal flag
reset	sei
		ldx #0xff
		txs
		cld

		; print text
		ldx #0xff
print	inx
		lda text,X
		sta textOut
		bne print

		; exit emulator with exit code 0
		lda #0
		sta exit

text 	dc "Hello World!", 0x0a, 0;

nmi		rti

irq		rti

		org 0xfffa
		; NMI vector
		dc <(nmi), >(nmi)
		; reset vector
		dc <(reset), >(reset)
		; IRQ/BRK vector
		dc <(irq), >(irq)
end
