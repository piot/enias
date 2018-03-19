    .org $0200

    ENIAS_SPRITES = $F400
    ENIAS_GAMEPAD = $FB00

    ;; Set X
    ldx #00
    lda sprite_x
    sta ENIAS_SPRITES,x

    ;; Set Y
    inx
    lda sprite_y
    sta ENIAS_SPRITES,x

    ;; Set tile index
    inx
    lda #101
    sta ENIAS_SPRITES,x

    ;; Update sprite_x
    ldx sprite_x
    inx
    stx sprite_x

    ;; Input logic
    lda ENIAS_GAMEPAD     ; a = ENIAS_GAMEPAD
    and #$04              ; result = a & 0x04
    beq dont_move_down    ; if(result == 0) { goto dont_move }
    inc sprite_y          ; sprite_y++
dont_move_down:
    lda ENIAS_GAMEPAD
    and #$08
    beq dont_move_up
    dec sprite_y
dont_move_up:

    rts

sprite_x:
    .byte $00

sprite_y:
    .byte 108

    .segment "GFX_TILES"
    .incbin "../assets/tiles.bin"

    .segment "GFX_PALETTE"
    .incbin "../assets/palette.bin"
