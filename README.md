# TTY Sokoban

![TTY Sokoban](ttysokoban.png)

A terminal-based Sokoban game written in C using the ncurses library.

## Description

TTY Sokoban is a simple implementation of the classic Sokoban puzzle game for terminal/TTY environments. The game features ASCII line drawing for walls, making it visually appealing in terminal environments.

## Requirements

- C compiler (like GCC)
- ncurses library

## Building

```
make
```

This will:
1. Compile the embed_levels tool
2. Generate the embedded_levels.h file from levels in the levels/ directory
3. Compile the game with the embedded levels

## Running the Game

```
./ttysokoban
```

Flags:

```
-a, --ascii    Use ASCII characters for walls instead of box drawing characters
-b, -bw        Black and white mode (disable colors)
```

## Game Controls

- Movement:
  - Arrow keys: Move the player
  - WASD keys: Alternative movement (W=up, A=left, S=down, D=right)
  - hjkl keys: Vi-style movement (h=left, j=down, k=up, l=right)
- r: Restart the current level
- n: Go to the next level
- p: Go to the previous level
- q: Quit the game


## Generating New Levels

The game uses the "sokohard" level format from https://github.com/mezpusz/sokohard

The game includes a script to generate new levels:

```
./generate_level.sh
```

## License

TTY Sokoban is Public Domain
