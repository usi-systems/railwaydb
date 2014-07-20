default: thorough

quick: $(wildcard *.tex)
	pdflatex main.tex

thorough: $(wildcard *.tex) main.bib
	pdflatex main.tex
	bibtex main
	pdflatex main.tex
	pdflatex main.tex

clean:
	-rm -f *~
	-rm -f main.{aux,bbl,blg,log,out,pdf}
