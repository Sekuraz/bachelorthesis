# Executables
latexmk = latexmk -outdir=build

pandoc = pandoc

# Main file name
MASTER_TEX = project.tex

project.pdf: $(MASTER_TEX) chapters/introduction.tex chapters/related_work.tex chapters/implementation.tex chapters/guide.tex chapters/examples.tex chapters/outlook.tex chapters/appendix.tex bibliography.bib
	$(latexmk) $(MASTER_TEX)


.PHONY: clean
clean:
	$(latexmk) -C

chapters/%.tex: chapters/%.md
	$(pandoc) --listings -t latex -o $@ $<

