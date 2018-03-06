# Enias

8-bit Fantasy Console!

[![Gitter](https://badges.gitter.im/Piot/enias.svg)](https://gitter.im/Piot/enias?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)

## Compile

Make sure that you have the sdl2 libraries installed.

```
brew install sdl2
```


```
cd src
./compile.sh

./enias examples/hackman/hackman.prg
```


## Memory Layout
All addresses are "indirect". The 16-bit value stored is used as the actual address.

| Memory Address  |  Size | Name                  | Description                                                             |
| --------------- |-------|-----------------------|-------------------------------------------------------------------------|
| Cpu             |       |                       |                                                                         |
| $0000-$00FF     | $0100 | ZP                    | Zero page (registers / scratch area)                                    |
| $0100-$01FF     | $0100 | Stack                 |                                                                         |
| Program         |       |                       |                                                                         |
| $0200-$AFFF     | $AE00 | Program area          |                                                                         |
| Graphics        |       |                       |                                                                         |
| $B000-$CFFF     | $2000 | tiles                 | 256 tiles (8x8) each pixel is 4-bit. (8 rows with 32 tiles) sprites and nametable.         |
| $D000-$EFFF     | $2000 | extended tiles        | 256 tiles (8x8) each pixel is 4-bit. Can only be used for sprites.      |
| $F000-$F3FF     | $0400 | nametable (default)   | $F000 = upper-left, $F100 = upper-right, $F200 = bottom-left, $F300 = bottom right. |
| $F400-$F4FF     | $0100 | sprites (default)     |                                                                         |
| $F500-$F52F     | $0030 | palette               | 16 entries with R,G,B (three octets)                                    |
| $F530-$F533     | $0004 | nametable scrolling   | $F530 x-offset (0-255), $F531 (0/1), $F532 y-offset (0-255), $F533 (0/1)|
| Audio           |       |                       |                                                                         |
| $F600-$F8BF     | $02C0 | 64 waves (x 11 octets)|                                                                         |
| $F8C0-$F97F     | $00B0 | 16 channels           |                                                                         |
| $F980-$F987     | $0008 | Effects               |                                                                         |
| Input           |       |                       |                                                                         |
| $FA00           | $0008 | Gamepad 4 x 2 octets  |                                                                         |
| $FA08           | $0001 | Keyboard input        |                                                                         |
| Reserved        |       |                       |                                                                         |
| $FB00-$FEFF     | $0500 | Reserved              |                                                                         |
| Vectors         |       |                       |                                                                         |
| $FFFC-$FFFD     | $0002 | Reset call address.   | Defaults to $0200                                                       |
| $FFFE-$FFFF     | $0002 | Frame call address.   | Defaults to $0200                                                       |

## Tiles
Each tile is 8x8 pixels, with four bits for each pixel. The pixel value is an actual index lookup into the palette. 0 (zero) is always considered to be transparent.

## Sprites

Usually starts at $FD00

| Octet Offset | Name    | bits     | Description       |
| -------------| ------- |----------|-------------------|
| 0            | x       | xxxxxxxx | from left to right|
| 1            | y       | yyyyyyyy | top to bottom     |
| 2            | tile    | tttttttt | tile index (each row is 32 tiles)|
| 3            | status  | vhp0000t | v: vertical flip, h: horizontal flip, p: priority (0 = behind background tiles) t: add 256 to tile index|

## Palette
16 palette entries. Each entry is:

| Octet Offset  | Name  |
|---------------|-------|
| 0             | red   |
| 1             | green |
| 2             | blue  |

## Name-table
The index for each tile to be displayed from top-left corner (32x28), 896 octets.

| Octet Offset | Size  | Name | Description                  |
|--------------|-------|------|------------------------------|
| $0000        | $0100 |      | "top-left". 32x28 index to tiles. (last $80 tiles not used).     |
| $0100        | $0100 |      | "top-right". 32x28 index to tiles. (last $80 tiles not used).    |
| $0200        | $0100 |      | "bottom-left". 32x28 index to tiles. (last $80 tiles not used).  |
| $0300        | $0100 |      | "bottom-right". 32x28 index to tiles. (last $80 tiles not used). |

## Input

Four gamepads, each has the following

| Octet Offset  | Name     | bits     |  Description |
|---------------|----------|----------|--------------|
| 0             | normal   | abstudlr | $80 A, $40 B, $20 Select, $10 Start, $08 Up, $04 Down, $02 Left, $01 Right
| 1             | extended | xylr0000 | $80 X, $40 Y, $20 Left trigger, $10 Right trigger

### Keyboard input

| Address       | Name     |   Description |
|---------------|----------|---------------|
| $FF08         | key      | Latest pressed key (ASCII) or 0 (zero) if no key pressed |

## Sound Chip

### Voice / Channel
16 voices. Each voice has the following info:

| Octet Offset | Name       | bits     | Description       |
| -------------| -----------|----------|-------------------|
| 0            | instrument | 000iiiii | index into instruments. (0-23)|
| 1            | note       | kkkkkkkk | 0: key off |
| 2            | velocity   | vvvvvvvv | |
| 3            | pan        | vvvvvvvv | 0: all left, 255: all right|
| 4            | pitch      | ffffffff | signed diff for key |
| 5            | volume     | vvvvvvvv | |
| 6            | fx1-gain   | ffffffff | fx1 = feed to reverb |
| 7            | fx2-gain   | ffffffff | fx2 = feed to delay  |
| 8            | low-pass   | ffffffff | |
| 9            | high-pass  | ffffffff | |
| A            | q-filter   | ffffffff | |


### Instrument (Sound definition)

24 Instruments.

| Octet Offset | Name                    | bits     | Description       |
| -------------| ------------------------|----------|-------------------|
| 0            | wave-index              | wwwwwwww | 0-64.  index into waves |
| 1            | attack                  | aaaaaaaa | time before maximum volume |
| 2            | decay                   | dddddddd | time before it reaches sustain volume |
| 3            | sustain                 | ssssssss | (optional?) volume level for the sustain part. |
| 4            | release                 | rrrrrrrr | (optional?) time before sound reaches zero volume. |


### Wave

64 Waves.

| Octet Offset | Name                    | bits     | Description       |
| -------------| ------------------------|----------|-------------------|
| 0            | sample-pointer (low)    | wwwwwwww | |
| 1            | sample-pointer (high)   | wwwwwwww | |
| 2            | sample-length (low)     | wwwwwwww | |
| 3            | sample-length (high)    | wwwwwwww | |
| 4            | loop                    | wll00000 | w: (0=8-bit samples, 1:16-bit samples). loop-mode (0: no loop, 1: forward, 2: ping-pong) |
| 5            | loop start (low)        | rrrrrrrr | loop start point |
| 6            | loop start (high)       | rrrrrrrr | |
| 7            | loop length (low)       | rrrrrrrr | loop length |
| 8            | loop length (high)      | rrrrrrrr | |
| 9            | relative_note_number    | rrrrrrrr | |
| A            | volume                  | rrrrrrrr | |



### Effect settings
One global effect system

| Octet Offset | Name                    | bits     | Description       |
| -------------| ------------------------|----------|-------------------|
| 0            | reverb-mix              | wwwwwwww | |
| 1            | delay-time              | wwwwwwww | |
| 2            | delay-reflect           | wwwwwwww | |
| 3            | delay-mix               | wwwwwwww | |


### Samples

| Octet Offset | Name                    |
| -------------| ------------------------|
| 0 - x        | 8/16-bit samples |
