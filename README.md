# TTY Sokoban

![TTY Sokoban](ttysokoban.png)

A terminal-based Sokoban game written in C using the ncurses library.

## Description

TTY Sokoban is a simple implementation of the classic Sokoban puzzle game for terminal/TTY environments.
The game features ASCII line drawing for walls, making it visually appealing in terminal environments.

## Features

- ASCII line drawing for walls
- Embedded levels (no external files needed)
- Simple and intuitive controls
- Support for multiple levels

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

Flags

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

## Level Format

The game uses the "sokohard" level format from https://github.com/mezpusz/sokohard

Character representations:
- `#`: Box (originally wall, now used for boxes)
- ` `: Empty space
- `@`: Player
- `+`: Player on goal
- `$`: Box (in level files)
- `*`: Box on goal
- `.`: Goal

Walls are displayed using ASCII line drawing characters for a more visually appealing experience.

## Generating New Levels

The game includes a script to generate new levels using the sokohard level generator:

```
./generate_level.sh
```

## Credits

This game uses level format and concepts from the sokohard project:
https://github.com/mezpusz/sokohard

## License

TTY Sokoban is Public Domain
