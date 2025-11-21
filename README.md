# Multiplayer Pong (C, SDL2, SDL_net)

A networked Pong clone written in C using SDL2 and SDL_net.  
The game supports up to four players over a local network, with one host acting as the server and the others connecting as clients.

> This project was originally developed as part of a university course.  
> All design and implementation of the game, networking and menus is my own.

---

## Features

- Classic Pong gameplay with paddles and a bouncing ball
- Up to 4 human players over UDP networking
- One host (server) controls:
  - game state (ball position, scores, game over, restart)
  - difficulty (ball speed etc.)
  - score limit / win condition
- Client menu to:
  - enter server IP address
  - connect to the game
- Simple in-game UI:
  - score display
  - game over screen
  - restart / exit handling

---

## Technologies

- **Language:** C
- **Libraries:** 
  - [SDL2] for graphics, input and timing
  - [SDL_net] for UDP networking
- **Build system:** `Makefile` (or describe how you build it)

---

## How to build

### Dependencies

You need SDL2 and SDL_net installed.

On Debian/Ubuntu-like systems, something like:

```bash
sudo apt-get install libsdl2-dev libsdl2-net-dev
