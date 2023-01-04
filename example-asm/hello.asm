constant cars = $2030

structure Frog {
    x: $02, y: $02,
    tileIndex: $01
}

structure Sprite {
    x: $2, y: $2,
    tileIndex: $1, animationOffset: $01,
    extra: $0a
}

structure Car {
    x: $2, y: $2,
    tileIndex: $1, animationOffset: $01,
    ignore0: $03,
    velocity: $2,
    extra: $04
}

structure Input {
    Up: $1, Down: $1,
    Left: $1, Right: $1,
    A: $01, B: $01,
    Start: $01, Select: $01
}

structure Box {
    x: $02, y: $02,
    x1: $02, y1: $02
}

constant InputAddr = $2620
constant FrogAddr = $2020
constant SpriteMemory = $2020

constant stepBackward = $0004
constant stepForward = $fffc

start:
    mov8 &[!hasDied], acu
    jne $00, &[!update_cars]

check_up_pressed:
    mov8 &[<Input>!InputAddr.Up], acu
    jeq $0000, &[!check_down_pressed]
    mov &[<Frog>!FrogAddr.y], r1
    add [!stepForward], r1
    mov acu, &[<Sprite>!FrogAddr.y]

check_down_pressed:
    mov8 &[<Input>!InputAddr.Down], acu
    jeq $0000, &[!check_left_pressed]
    mov &[<Frog>!FrogAddr.y], r1
    add [!stepBackward], r1
    mov acu, &[<Sprite>!FrogAddr.y]

check_left_pressed:
    mov8 &[<Input>!InputAddr.Left], acu
    jeq $0000, &[!check_right_pressed]
    mov &[<Frog>!FrogAddr.x], r1
    add [!stepForward], r1
    mov acu, &[<Sprite>!FrogAddr.x]

check_right_pressed:
    mov8 &[<Input>!InputAddr.Right], acu
    jeq $0000, &[!check_input_end]
    mov &[<Frog>!FrogAddr.x], r1
    add [!stepBackward], r1
    mov acu, &[<Sprite>!FrogAddr.x]

check_input_end:

update_cars:
    ; -- load cars offset into the acu
    mov8 &[!carOffset], acu
    ; if car #3 -> !exit from loop
    jeq $0020, &[!update_cars_end]

    ; -- save current carOffset
    mov acu, r5

    ; -- calculate car position save in r4
    add [!cars], r5
    mov acu, r4

    ; -- mov xpos into r1
    mov &acu, r1

    ; -- calculate position of speed value
    add $0009, acu
    ; -- mov speed to r2
    mov &acu, r2

    ; -- calculate new x move to r3
    add r1, r2
    mov acu, r3

;; Negative bounds detection
    and acu, $8000
    jeq $0000, &[!positive_bounds_check]
    mov $00f0, r3
    mov [!end_bounds_check], ip

positive_bounds_check:
    mov r3, acu
    jgt $00f0, &[!end_bounds_check]
    mov $0000, r3

end_bounds_check:

update_car_position:
    ; -- set new xpos to car
    mov r3, &r4

start_car_collision_check:
    ;; mov car offset to acu
    mov r5, r1
    rsf r1, $04
    psh $0
    cal [!collision]
    jeq $00, &[!end_car_collision_check]
    mov $08, &[<Frog>!FrogAddr.x]
    mov $08, &[<Frog>!FrogAddr.y]
    mov $08, &[<Frog>!FrogAddr.tileIndex]
    mov8 $01, &[!hasDied]

end_car_collision_check:

end_update_cars_loop:
    ; -- calculate and set new carOffset
    add $0010, r5
    movl acu, &[!carOffset]

    ; -- jump to start of loop
    mov [!update_cars], ip

update_cars_end:
    mov $0000, &[!carOffset]

end_of_game_logic:
    mov8 $01, &[!hasEnded]

end_of_game_logic_loop:
    mov [!end_of_game_logic_loop], ip





;; bool collision(uint16 spriteIndex)
collision:
    ;; r1 contains spriteIndex

    ;; Build Frog Box struct
    mov &[<Frog>!FrogAddr.x], r3
    mov &[<Frog>!FrogAddr.y], r4
    mov r3, &[<Box>!FrogBox.x]
    mov r4, &[<Box>!FrogBox.y]
    add $0008, r3
    mov acu, &[<Box>!FrogBox.x1]
    add $0008, r4
    mov acu, &[<Box>!FrogBox.y1]

    ;; Build Other Box Struct
    lsf r1, $04
    add [!cars], r1
    ;; Move baseAddress to r1
    mov acu, r1
    
    ;; Get address of x
    add [<Sprite>$0.x], acu
    mov &acu, r2
    mov r2, &[<Box>!OtherBox.x]

    ;; Get address of y
    mov r1, acu
    add [<Sprite>$0.y], acu
    mov &acu, r3
    mov r3, &[<Box>!OtherBox.y]

    ;; Calculate x1
    add $0008, r2
    mov acu, &[<Box>!OtherBox.x1]

    ;; Calculate y1
    add $0008, r3
    mov acu, &[<Box>!OtherBox.y1]

corner0:
    ;; ax < bx
    mov &[<Box>!FrogBox.x], r1
    mov &[<Box>!OtherBox.x], acu
    jlt r1, &[!corner1]

    ;; ax >= bx1
    mov &[<Box>!OtherBox.x1], acu
    jge r1, &[!corner1]

    ;; ay < by 
    mov &[<Box>!FrogBox.y], r1
    mov &[<Box>!OtherBox.y], acu
    jlt r1, &[!corner1]
    
    ;; ay >= by1
    mov &[<Box>!OtherBox.y1], acu
    jge r1, &[!corner1]

    mov [!collided], ip

corner1:
    ;; ax1 <= bx
    mov &[<Box>!FrogBox.x1], r1
    mov &[<Box>!OtherBox.x], acu
    jle r1, &[!corner2]

    ;; ax1 > !bx1
    mov &[<Box>!OtherBox.x1], acu
    jgt r1, &[!corner2]

    ;; ay < by 
    mov &[<Box>!FrogBox.y], r1
    mov &[<Box>!OtherBox.y], acu
    jlt r1, &[!corner2]
    
    ;; ay >= by1
    mov &[<Box>!OtherBox.y1], acu
    jge r1, &[!corner2]

    mov [!collided], ip

corner2:
    ;; ax < bx
    mov &[<Box>!FrogBox.x], r1
    mov &[<Box>!OtherBox.x], acu
    jlt r1, &[!corner3]

    ;; ax >= bx1
    mov &[<Box>!OtherBox.x1], acu
    jge r1, &[!corner3]

    ;; ay1 > !by1
    mov &[<Box>!FrogBox.y1], r1
    mov &[<Box>!OtherBox.y1], acu
    jgt r1, &[!corner3]

    ;; ay1 <= by
    mov &[<Box>!OtherBox.y], acu
    jle r1, &[!corner3]

    mov [!collided], ip

corner3:
    ;; ax1 <= bx
    mov &[<Box>!FrogBox.x1], r1
    mov &[<Box>!OtherBox.x], acu
    jle r1, &[!end_of_collision_routine]

    ;; ax1 > !bx1
    mov &[<Box>!OtherBox.x1], acu
    jgt r1, &[!end_of_collision_routine]

    ;; ay1 > !by1
    mov &[<Box>!FrogBox.y1], r1
    mov &[<Box>!OtherBox.y1], acu
    jgt r1, &[!end_of_collision_routine]

    ;; ay1 <= by
    mov &[<Box>!OtherBox.y], acu
    jle r1, &[!end_of_collision_routine]

    mov [!collided], ip

end_of_collision_routine:
    mov $00, acu
    ret

collided:
    mov $01, acu
    ret

after_frame:
    psh acu
    mov8 &[!hasEnded], acu
    jeq $0000, &[!after_frame_2]
    mov8 $00, &[!hasEnded]
    pop acu
    pop r8
    psh [!start]
    rti


after_frame_2:
    pop acu
    rti

data8 hasDied = { $00 }
data8 hasEnded = { $00 }
data8 carOffset = { $00 }
data16 FrogBox = { $0000, $0000, $0000, $0000 }
data16 OtherBox = { $0000, $0000, $0000, $0000 }