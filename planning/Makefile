.SUFFIXES: .pdf .tex

.tex.pdf:
	pdflatex $<

planning: plan.tex
	mv plan.pdf Car_Boran_Masterthesisplan_2011-2012.pdf

clean:
	rm *.aux
	rm *.log
	rm *~
