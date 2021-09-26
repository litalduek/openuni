;>>>>>>>>>>>>>>>>>>>>Long lines<<<<<<<<<<<<<<<<<<<
.asciz "This line will be morrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrre than 80 characters"
;me tooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
.dh 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        
;>>>>>>>>>>>>>>>>>>>>Illegal registers<<<<<<<<<<<<<<<<<<<        
       	SA: add     $+0 ,,$1 ,$2  
        sub     $1 ,$-2 $5
        and     $5 ,$1 ,$32
        move    $+19,$11
        mvhi    $11,$-10
        addi    $-24  ,  123  ,  $25
        bne     $32 ,   $23,    label   
        lb      $32  ,  212  ,  $25    
        lw      $-28,    -30,    $29
        sw      $30,-1111,$+31 
        or      $9 ,$10,7
        nor     12,$13,$14 

;>>>>>>>>>>>>>>>>>>>>Illegal commas<<<<<<<<<<<<<<<<<<<        
        add     $0 ,$1 ,$2 , 
        sub     $3 ,,$4 ,$5
        or      $9 ,$1,0,$11
        nor     $12,$13$14 
        move    $15.$16
        mvlo    $21,,$22
        addi    $24  ,  123 , ,  $25
        subi    $26 ,   0   ,   $2,7  
        andi    $28,    -3,0,    $29,
        ori     ,$30,-32768,$31 
        nori    $17,327,67,$20
        bne     $23 ,   $23,    label  , 
        bgt     $20,    $15,    ,label
        sb      $26, ,   0   ,   $27
        lw      $28,    ,-30,    $29
        lh      $17,,32767,$20
        sh      $26 ,   0   ,   $2,7  
        jmp     $7,
        jmp     ,$7
        .db     0,5,-3,+80, 1,1 ,-128,127 , 
        .dh     0,5,-3,+80, 1,,1 ,-32768,32767 

;>>>>>>>>>>>>>>>>>>>>Invalid immeds<<<<<<<<<<<<<<<<<<<        
        addi    $16  ,  43.2  ,  $1
        subi    $14 ,   +-1   ,   $2  
        andi    $13,    --1,    $3
        lb      $12  ,  1.23  ,  $3   
        sb      $17 ,   -+1   ,   $6
        sh      $11 ,   !1   ,   $4

;>>>>>>>>>>>>>>>>>>>>Invalid operands<<<<<<<<<<<<<<<<<<<        
        add     $0 ,$1 ,$2 ,$2 
        sub     $3 ,$4 
        move    $15
        addi    $24   ,  $25
        subi    $26 ,   0   ,   $27  $3 
        andi    $28, 
        ori     -32768
        bne     $23 ,       label   
        beq     $16,    $6,    
        lb      $24  ,  123     
        jmp     label, label
        jmp     $7,$3
        .asciz  "no quatation mark
        .asciz  me to!"
        .asciz  so do i!

;>>>>>>>>>>>>>>>>>>>>Invalid commands<<<<<<<<<<<<<<<<<<<        
        end     $6 ,$7 ,$8
        mve    $15,$16
        orri    $28,    -30,    $29
        sw!      $30,-32768,$31
        call&    label
        .assci   "HELLO"

;>>>>>>>>>>>>>>>>>>>>Invalid labels<<<<<<<<<<<<<<<<<<<        
label: stop
label:  add     $0 ,$1 ,$2
label!:  add     $0 ,$1 ,$2
$label:  move    $15,$16
bad label:  add     $0 ,$1 ,$2
longgggggggggggggggggglabelllllllllllllllllllll:  move    $15,$16
2label:  add     $0 ,$1 ,$2
label  move    $15,$16
add:  add     $0 ,$1 ,$2
asciz:  move    $15,$16
:       move    $15,$16
        .extern label
label:  move    $15,$16
