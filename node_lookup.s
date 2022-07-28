//file header
    .arch armv6     //armv6 architecture
    .arm            //arm 32-bit IS
    .fpu vfp        //floating point co-processor
    .syntax unified //modern syntax


    //.data         //uncomment if needed

    .text           //start of text segment

    .global node_lookup               //make node_lookup global for linking to
    .type   node_lookup, %function    //define node_lookup to be a function
    .equ 	FP_OFF, 24 	  // fp offset distance from sp 
node_lookup:	
// function prologue

push {r4-r8,fp,lr}
add   fp, sp, FP_OFF
    cmp r0,0
    beq .Lfin
ldr r4, [r0]  //year
ldr r5, [r0,#4] //month
ldr r6, [r0,#8] //day
ldr r7, [r0,#12] //hour from struct 
ldr r8, [fp,#4] //hour in variable
   
    

.Loop:  
        cmp r1,r4
        bne .Lchange
        cmp r2,r5
        bne .Lchange
        cmp r3,r6
        bne .Lchange
        cmp r8,r7
        beq .Lfin

.Lchange: 
       ldr r0,[r0,#24]
       ldr r4,[r0]
       ldr r5,[r0,#4]
       ldr r6,[r0,#8]
       ldr r7,[r0,#12]
       b .Loop

// function epilogue

.Lfin: 

    sub  sp, fp, FP_OFF   // restore sp
    pop  {r4-r8,fp, lr}      // restore saved registers
    bx   lr

// function footer - do not edit below
    .size node_lookup, (. - node_lookup) // set size for function

//file footer
    .section .note.GNU-stack,"",%progbits // stack/data non-exec (linker)
.end

