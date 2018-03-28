# Executables
latexmk = latexmk -outdir=build

pandoc = pandoc

# Main file name
MASTER_TEX = main-english.tex

main-english.pdf: $(MASTER_TEX) chapters/abstract.tex chapters/introduction.tex chapters/related_work.tex chapters/openmp_tasking.tex chapters/overview.tex chapters/runtime_nodes.tex chapters/worker_nodes.tex chapters/outlook.tex bibliography.bib
	$(latexmk) $(MASTER_TEX)


.PHONY: clean
clean:
	$(latexmk) -C

chapters/%.tex: chapters/%.md
	$(pandoc) --listings -t latex -o $@ $<

