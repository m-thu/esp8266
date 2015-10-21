PROJECTS=toggle

.PHONY: all clean $(PROJECTS)

all: $(PROJECTS)

$(PROJECTS):
	$(MAKE) -C $@

clean:
	for dir in $(PROJECTS); do \
		$(MAKE) -C $$dir clean; \
	done
