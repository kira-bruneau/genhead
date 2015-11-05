CFLAGS = -I./src
OBJECTS = main genhead io

all: genhead

.PHONY: genhead
genhead: bin/genhead
bin/genhead: $(addprefix bin/, $(addsuffix .o, $(OBJECTS)))
	$(CC) -o $@ $^ $(LDFLAGS)

bin/%.o: src/%.c
	@mkdir -p $(shell dirname $@)
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf bin/

.PHONY: all
.PHONY: main
.PHONY: clean
