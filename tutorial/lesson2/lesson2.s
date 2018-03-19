    .org $0200

    rts

    .segment "GFX_TILES"
    .incbin "../assets/enias-font.bin"

    .segment "GFX_PALETTE"
    .incbin "../assets/default_palette.bin"
