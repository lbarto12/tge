# TGE - Terminal Game Engine

## Examples
![tetris](./md-imgs/tetris.gif)
![cube](./md-imgs/cube.gif)
![snake](./md-imgs/snake.gif)

## What is this?
This is a complete set of multi-platform terminal rendering, input, event, and life-cycle management tools.

## How to use it.
Still need to write docs, but it will be cool :)

## How it works.
The secret sauce is in how the characters are rendered to the screen. The program uses a custom class for
drawing to the terminal. This class emulates a console buffer per-character, and allows you to write to the *next*
frame without disturbing the current one - then swaps the buffers, like in traditional computer graphics :)
The primary difference is that the buffer-swap *only* redraws characters that have changed since the last frame,
significantly reducing the required number of cursor movements and character prints. This obviously doesn't mitigate
the strain of rendering subsequent, unique frames, but it allows for much smoother movement and execution during
procedural game logic, and is designed for that. No need to do more work than you have to :)


