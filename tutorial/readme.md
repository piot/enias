# The Enias Tutorial

## Lesson 0 - installing Enias
1. Clone this repo.
2. Install the `ca65` compiler:
   2.1 Clone [https://github.com/cc65/cc65](https://github.com/cc65/cc65)
   2.2 `cd` into the directory and run `make`
   2.3 Finally, to make the binaries available on your system, run `sudo make avail`
3. Install SDL2.
4. Go to `/src/` and run `$ ./compile`

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

1. Enias only support 16 colors at the same time. To define those colors, a palette file is used. An example palette can be found in [/assets/default_palette.bin](assets/default_palette.bin). It contains 16 RGB (24-bits each) colors.

2. Sprites and symbols (font characters, map tiles, etc.) all share the same pixel definition. It can be found in the file [/assets/enias-font.bin](assets/enias-font.bin)

3.

## Lesson 3 - moving a sprite
