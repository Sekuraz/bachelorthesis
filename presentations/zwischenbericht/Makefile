PANDOC = /usr/sbin/pandoc
PARAMS = -t beamer -o slides.pdf

slides.pdf: slides.md
	$(PANDOC) $(PARAMS) slides.md

.PHONY: clean
clean:
	rm slides.pdf
