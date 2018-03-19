# The Enias Tutorial

## Lesson 0 - installing Enias
1. Install the `ca65` compiler, either through `brew install cc65` or manually:
   1. Clone [https://github.com/cc65/cc65](https://github.com/cc65/cc65)
   2. `cd` into the directory and run `make`
   3. Finally, to make the binaries available on your system, run `sudo make avail`
2. Install SDL2 via brew or from source (https://www.libsdl.org/download-2.0.php).
3. Clone this repo.
4. Go to `<path-to-enias>/src/` and run `$ ./compile`

## Lesson 1 - a minimal project
1. Create an empty project directory
2. Add a file called `lesson1.s` and enter the following 6502 assembler:

```6502
    .org $0200

    rts
```

3. Compile to an object file with

```bash
$ ca65 lesson1.s
```

4. Then link it (using the Enias memory [layout config file](../asm/enias.cfg))

```bash
$ ld65 -C <path-to-enias>/asm/enias.cfg lesson1.o
```

5. This will produce a `a.out` executable:

```bash
$ ls
a.out
lesson1.s
lesson1.o
```

6. Finally - run it inside enias

```bash
$ <path-to-enias>/src/enias a.out
```

## Lesson 2 - rendering a sprite

1. Enias only support 16 colors at the same time. To define those colors, a palette file is used. An example palette can be found in [/assets/palette.bin](assets/palette.bin). It contains 16 RGB (24-bits each) colors.

2. Sprites and symbols (font characters, map tiles, etc.) all share the same pixel definition. It can be found in the file [/assets/tiles.bin](assets/tiles.bin)

3. The `.segment` lets you specify to ld65 where a certain resource is stored in memory. Adding the following statements will let Enias find the pixel data and the palette.

```6502
    .org $0200

    rts

    .segment "GFX_TILES"
        .incbin "../assets/tiles.bin"

    .segment "GFX_PALETTE"
    .incbin "../assets/palette.bin"
```

4. Enias supports 64 sprites of 8x8 pixels each. To actually render a sprite we have to define its position (x and y) plus its tile index (its position in GFX_TILES).

<img src="../assets/tiles.png">

For example, to render the letter 'E' in the middle of the screen we have to set one of the sprites' X to 124, Y to 108, and tile to 96. It doesn't really matter which one of the sprites we use (0 - 63) except for controlling the order of rendering.

5.

## Lesson 3 - moving a sprite
