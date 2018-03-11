.proc sprite_move
	; Move sprite to position
	; a: sprite index
	; x: x-position
	; y: y-position
	clc
	asl ; sprite_index * 4
	asl
	sta $fd
	stx $fe

	ldx $fd ; sprite-offset
	lda $fe ; x-position
	sta ENIAS_SPRITES,x ; sprite x-position
	inx

	tya
	sta ENIAS_SPRITES,x ; sprite y-position

	rts
.endproc

.proc sprite_set_tile
	; a: sprite index
	; y: tile-index
	clc
	asl
	asl
	clc
	adc #$02  ; skip x and y position
	tax

	tya
	sta ENIAS_SPRITES,x ; tile-index
	inx

	lda #$ff
	sta ENIAS_SPRITES,x ; status

	rts
.endproc

.proc sprite_set_status
	; a: sprite index
	; y: status
	asl
	asl
	clc
	adc #$03  ; skip x, y position and tile index
	tax

	tya
	sta ENIAS_SPRITES,x ; status

	rts
.endproc


.proc sprite_move_2x2
	; a start-sprite
	; x: x-coord
	; y: y-coord
	SPRITE_INDEX_REG=$f0
	X_REG=$f1
	Y_REG=$f2
	sta SPRITE_INDEX_REG
	stx X_REG
	sty Y_REG

	jsr sprite_move ; upper-left

	lda X_REG
	adc #$08 ; x += 8
	tax
	ldy Y_REG
	inc SPRITE_INDEX_REG
	lda SPRITE_INDEX_REG
	jsr sprite_move ; upper-right

	ldx X_REG
	lda Y_REG
	adc #$08 ; y += 8
	tay
	sty Y_REG

	inc SPRITE_INDEX_REG
	lda SPRITE_INDEX_REG
	jsr sprite_move ; lower-left


	lda X_REG
	adc #$08 ; x += 8
	tax
	ldy Y_REG
	inc SPRITE_INDEX_REG
	lda SPRITE_INDEX_REG
	jsr sprite_move ; lower-right

.endproc
