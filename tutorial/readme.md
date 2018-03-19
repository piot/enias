# The Enias Tutorial

## Lesson 0 - installing Enias
1. Clone this repo.
2. Install the `ca65` compiler.
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



## Lesson 3 - moving a sprite
