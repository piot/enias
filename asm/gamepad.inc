
.proc gamepad_delta_and_buttons
	; result:
	; x: delta
	; y: delta
	; a: buttons
	lda ENIAS_GAMEPAD

	ldx #0
	ldy #0

check_right:
	lsr ; RIGHT in carry
	bcc check_left
	inx

check_left:
	lsr ; LEFT in carry
	bcc check_down
	dex

check_down:
	lsr ; DOWN in carry
	bcc check_up
	iny

check_up:
	lsr ; UP in carry
	bcc check_buttons
	dey

check_buttons:

	rts
.endproc
