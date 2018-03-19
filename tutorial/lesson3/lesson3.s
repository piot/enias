    .org $0200

    ENIAS_SPRITES = $F400

;; Set X
    ldx #00
    lda sprite_x                ; a = sprite_x
    sta ENIAS_SPRITES,x

;; Set Y
    inx
    lda #108
    sta ENIAS_SPRITES,x

;; Set tile index
    inx
    lda #101
    sta ENIAS_SPRITES,x

    ;; Update sprite_x
    ldx sprite_x
    inx
    stx sprite_x

    rts

sprite_x:
    .res $00

    .segment "GFX_TILES"
    .incbin "../assets/tiles.bin"

    .segment "GFX_PALETTE"
    .incbin "../assets/palette.bin"
