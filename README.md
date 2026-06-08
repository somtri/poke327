# Poke327

Poke327 is a terminal Pokemon-style adventure written in C and C++. It
generates a large explorable world with trainers, wild encounters, turn-based
battles, shops, healing centers, a six-Pokemon party, and PC storage.

This is an independent educational fan project. It is not affiliated with or
endorsed by Nintendo, Game Freak, Creatures, or The Pokemon Company.

## Features

- Procedurally generated 401 by 401 world
- Trainer pathfinding and several trainer movement behaviors
- Wild Pokemon encounters, capture rates, and party storage
- Turn-based battles with move priority, accuracy, STAB, and type effectiveness
- PokeBucks, trainer payouts, Potions, Revives, and Poke Balls
- PokeMarts and Pokemon Centers
- Built-in compact Pokedex, with optional support for the original CSV dataset
- Deterministic runs with `--seed`

## Requirements

The native build targets a Unix-like terminal because the interface uses
`ncurses`.

On Ubuntu or Debian:

```bash
sudo apt update
sudo apt install build-essential libncurses-dev
```

On Windows, use WSL with Ubuntu, or use the Docker instructions below.

## Build And Run

```bash
make
./poke327
```

Run with a repeatable random seed:

```bash
./poke327 --seed 12345
```

Display command-line options with `./poke327 --help`.

Run the non-interactive data and startup check:

```bash
make smoke-test
```

The game needs a terminal at least 80 columns wide and 24 rows tall.

## Docker

```bash
docker build -t poke327 .
docker run --rm -it poke327
```

## Controls

| Key | Action |
| --- | --- |
| Arrow keys, numeric keypad, or vi keys | Move |
| `5`, space, or `.` | Wait one turn |
| `>` | Enter a PokeMart or Pokemon Center while standing on it |
| `B` | Open the bag |
| `t` | List trainers |
| `Q` | Quit |

Battle and building screens show their available keys in the terminal.

## Pokemon Data

No download is required. When the original course CSV database is unavailable,
the game loads an included compact set of Pokemon and moves.

To use a compatible full CSV database, set `POKE327_DATA_DIR` to the directory
containing:

```text
pokemon.csv
moves.csv
pokemon_moves.csv
pokemon_species.csv
experience.csv
type_names.csv
pokemon_stats.csv
stats.csv
pokemon_types.csv
```

The game also checks `./data/csv`, `./pokedex/pokedex/data/csv`,
`/share/cs327/pokedex/pokedex/data/csv`, and the original `~/.poke327` path.

## Project Structure

- `poke327.cpp`: world and map generation, turn scheduling, and program entry
- `character.cpp`: player and trainer movement
- `io.cpp`: ncurses UI, battles, shops, centers, and encounters
- `pokemon.cpp`: Pokemon generation and stats
- `db_parse.cpp`: external CSV loader and built-in fallback data
- `heap.c`: Fibonacci heap used by pathfinding and turn scheduling

## Verification

GitHub Actions builds the game and runs `make smoke-test` on Ubuntu for every
push and pull request.
