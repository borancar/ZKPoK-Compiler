all: main.ihx
.SUFFIXES: .rel .c

.c.rel:
	sdcc-sdcc -mmcs51 -c $<

main.ihx: main.rel lib.rel
	sdcc-sdcc -mmcs51 --iram-size 128 --xram-size 32768 --code-size 40960 --model-small main.rel lib.rel

clean:
	rm *.asm *.lnk *.map *.mem *.rel *.rst *.sym *.lst *~ *.aux *.bbl *.log *.blg

