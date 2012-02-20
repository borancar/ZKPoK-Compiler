all: references pdf
	cp thesis.pdf ImplementationZKPK.pdf

references: references.bib thesis.tex
	pdflatex thesis.tex
	bibtex thesis.aux

pdf: thesis.tex references
	pdflatex thesis.tex
	pdflatex thesis.tex

clean:
	rm -f _region_.tex
	rm -f *.log
	rm -fr *.prv
	rm -fr auto
	rm -f *~
	rm -f *.bbl
	rm -f *.blg
	rm -f *.lof
	rm -f *.out
	rm -f *.aux
	rm -f *.lot
	rm -f *.toc
