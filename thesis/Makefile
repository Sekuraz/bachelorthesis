# Executables
latexmk = latexmk -outdir=build

pandoc = pandoc

# Main file name
MASTER_TEX = main-english.tex

main-english.pdf: $(MASTER_TEX) chapters/abstract.tex chapters/introduction.tex chapters/related_work.tex chapters/implementation.tex chapters/openmp_tasking.tex chapters/variables_and_source.tex chapters/examples.tex chapters/limitations.tex chapters/outlook.tex chapters/appendix.tex bibliography.bib
	$(latexmk) $(MASTER_TEX)


.PHONY: clean
clean:
	$(latexmk) -C

chapters/%.tex: chapters/%.md
	$(pandoc) --listings -t latex -o $@ $<

