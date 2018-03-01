# Executables
latexmk = latexmk

pandoc = pandoc


# Main file name
MASTER_TEX = main-english.tex
LITERATURE = bibliography.bib

date=$(shell date +%Y%m%d%H%M)

main-english.pdf: $(MASTER_TEX) introduction.tex
	$(latexmk) $(MASTER_TEX)


.PHONY: clean
clean:
	$(latexmk) -C

introduction.tex: introduction.md
	pandoc -t latex -o introduction.tex introduction.md
