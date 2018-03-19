    .org $0200

    ENIAS_SPRITES = $F400       ; Memory address of the sprites

    ldx #00                     ; No offset
    lda #124                    ; X position of the sprite
    sta ENIAS_SPRITES,x

    inx                         ; Increase the offset to 1
    lda #108                    ; Y position of the sprite
    sta ENIAS_SPRITES,x

    inx                         ; Increase the offset to 2
    lda #101                    ; Tile index
    sta ENIAS_SPRITES,x

    rts

    .segment "GFX_TILES"
    .incbin "../assets/tiles.bin"

    .segment "GFX_PALETTE"
    .incbin "../assets/palette.bin"
