CC = gcc
CFLAGS = -Wall -Wextra -pedantic -g
LDFLAGS = -lncurses

# Default target
all: embed_levels ttysokoban

# Generate embedded_levels.h from level files
embedded_levels.h: embed_levels levels/*.sok
	./embed_levels

# Build the C generator program
embed_levels: embed_levels.c
	$(CC) $(CFLAGS) -o $@ $<

# Build the ttysokoban executable
ttysokoban: ttysokoban.c embedded_levels.h
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Run the game
run: ttysokoban
	./ttysokoban

# Update levels and rebuild
update: embed_levels
	./embed_levels
	$(MAKE) ttysokoban

# Clean generated files
clean:
	rm -f ttysokoban embedded_levels.h embed_levels
	rm -rf *.dSYM

.PHONY: all run clean update
