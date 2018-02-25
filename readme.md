# Enias

8-bit Fantasy Console!

## Compile

```
cd src
./compile.sh

./enias examples/hackman/hackman.prg
```

## Memory Layout

| Memory Address  | Name            | Description |
| --------------- | ----------------|---|
| $FE00-$FE01     | tiles           |   |
| $FE02-$FE03     | sprite-table    |   |
| $FE04-$FE05     | palette         |   |
| $FE06-$FE07     | name-table      |   |
| $FF00-$FF05     | input           |   |

## Tiles
Each tile is 8x8 pixels, with four bits for each pixel. The pixel value is an actual index lookup into the palette. 0 (zero) is always considered to be transparent.

## Sprites

| Octet Offset | Name    | bits     | Description |
| -------------| ------- |----------|-------------------|
| 0            | x       | xxxxxxxx | from left to right|
| 1            | y       | yyyyyyyy | top to bottom     |
| 2            | tile    | tttttttt | tile index (each row is 32 tiles)|
| 3            | status  | vh000ptt | v: vertical flip, h: horizontal flip, p: priority (0 = behind background tiles)|

## Palette
16 palette entries. Each entry is:

| Octet Offset  | Name |
| ------------- | ---- |
| 0             | red   |
| 1             | green |
| 2             | blue  |

## Name-table
The index for each tile to be displayed from top-left corner (32x28), 896 octets.

## Input
Four gamepads, each has the following

| Octet Offset  | Name     | bits     |  Description |
| ------------- | ----     |----------|-------------|
| 0             | normal   | abstudlr | $80 A, $40 B, $20 Select, $10 Start, $08 Up, $04 Down, $02 Left, $01 Right
| 1             | extended | xylr0000 | $80 X, $40 Y, $20 Left trigger, $10 Right trigger
