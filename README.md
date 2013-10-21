chip8-sdl
=========

![Space Invaders](http://spittiepie.com/img/up/138231589335e.png)

Simple chip8 emulator, wrote in spaghetti C and with SDL for graphic and input.
It implements all the opcodes (but 0NNN), but it's not bug free.
It run many games/programs, but not all yet.

How to run
=========
* Download main.c
* Compile with "gcc main.c -std=c99 -lSDL -o chip8-sdl"
* Run with "./chip8-sdl <ROM NAME>"

Tested under Linux, but it should compile everywhere it's avaiable a C compiler and SDL.


To-Do
=========
* Fix opcodes
* Refactor code
* Implement some kind of timer (right now the emulator just run as fast as it can, if you domment "SDL_Delay(1);")
* Add sound
* Add gui (?)
* Use OpenGL instead of SDL for drawing
* Implement super/mega chip8 opcodes
* Better performance

Programs/games
=========
http://www.chip8.com/?page=109 contain probably every program/game ever written for the chip8.

Tutorial/documentation
=========
To write this emulator, I've read tutorial and documentation from the following sources:
* http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
* http://www.multigesture.net/wp-content/uploads/mirror/goldroad/chip8.shtml
* www.emulator101.com
* http://web-beta.archive.org/web/20060424124226/http://djxander.artists.mpfspromotions.com/CHIP8.htm
* https://en.wikipedia.org/wiki/CHIP-8
* http://www.emutalk.net/threads/19894-Chip-8
* http://lazyfoo.net/SDL_tutorials/index.php
* http://wiki.libsdl.org/FrontPage

Thanks!
