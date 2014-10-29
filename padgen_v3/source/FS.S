.arm
.section .text

.global FileOpen
.type FileOpen, %function
//FileOpen(wchar_t* fname)
FileOpen:

    f_handle   = 0x20         

    STMFD   SP!, {R3-R12,LR}
    SUB     SP, SP, #0x30

    add    r7, r0, #0      

    LDR     R5, =0x809797C
    LDR     R5, [R5]
    ADD     R5, #8
    LDR     R1, =0x2EA0
    ADD     R0, R5, R1
    ADD     R1, SP, #8
    LDR     R4, =0x8061451
    BLX     R4

    MOV     R3, #0
    STR     R3, [SP,#0x1C]
    STR     R3, [SP]
    STR     R3, [SP,#4]
    ADD     R0, SP, #0x10
    MOV     R1, R5
    LDR     R2, [SP,#8]
    LDR     R3, [SP,#0xC]
    LDR     R4, =0x8063F91
    BLX     R4
    LDR     R6, [SP,#0x1C]
    ADD     R0, SP, #0x24
    MOV     R1, #4
    STR     R1, [R0]
    MOV     R1, #0x1C
    STR     R1, [R0,#8]
    add     r1, r7, #0
    STR     R1, [R0,#4]
    MOV     R0, #0
    STR     R0, [SP, #f_handle]
    MOV     R3, #7  // FILE_READ | FILE_WRITE | FILE_CREATE
    STR     R3, [SP]
    STR     R0, [SP,#4]
    ADD     R1, SP, #0x20
    MOV     R2, #0
    ADD     R3, SP, #0x24
    LDR     R10, =0x8084739
    MOV     R0, R6

    BLX     R10

    LDR     R0, [SP, #f_handle]

    ADD     SP, SP, #0x30
    LDMFD   SP!, {R3-R12,PC}
    bx	lr


.global FileRead
.type FileRead, %function
//void FileRead(u32 file_handle, void* in_buf, u32 size)
FileRead:
    f_read_call   = 0x38

    STMFD   SP!, {R3-R12,LR}
    SUB     SP, SP, #0x20

    ADD     R3, SP, #0x10
    STR     R2, [SP,#0x8]
    LDR     R2, =0x80944C8
    STR     R2, [R3]
    STR     R1, [R3,#4]
    STR     R3, [SP,#0x0]
    MOV     R1, #0
    STR     R1, [SP,#0x4]
    ADD     R1, SP, #0xC
    mov     R2, #0
    MOV     R3, #0
    LDR     R4, [R0]
    LDR     R4, [R4, #f_read_call]

    BLX     R4

    ADD     SP, SP, #0x20  
    LDMFD   SP!, {R3-R12,PC}
    bx	lr


//[19:15] <@sm> in FileRead one of those two arguments at the end is the offset
//[19:15] <@sm>     mov     R2, #0
//[19:15] <@sm>     MOV     R3, #0
//[19:16] <@sm> so you could write FileReadOffset and just use one of those as an argument
//[19:16] <@sm> I think it's R2 but not 100% sure never actually used it

.global FileWrite
.type FileWrite, %function
//void FileWrite(u32 file_handle, void* write_buf, u32 size)
FileWrite:
    f_write_call   = 0x3C

    STMFD   SP!, {R3-R12,LR}
    SUB     SP, SP, #0x20

    ADD     R3, SP, #0x10
    STR     R2, [SP,#0x8]
    LDR     R2, =0x8094490
    STR     R2, [R3]
    STR     R1, [R3,#4]
    STR     R3, [SP,#0x0]
    MOV     R1, #0
    STR     R1, [SP,#0x4]
    ADD     R1, SP, #0xC
    mov     R2, #0
    MOV     R3, #0
    LDR     R4, [R0]
    LDR     R4, [R4, #f_write_call]

    BLX     R4

    ADD     SP, SP, #0x20  
    LDMFD   SP!, {R3-R12,PC}
    bx	lr


.global FileWriteOffset
.type FileWriteOffset, %function
//void FileWriteOffset(u32 file_handle, void* write_buf, u32 size, u32 offset)
FileWriteOffset:
    f_write_call   = 0x3C

    STMFD   SP!, {R4-R11,LR}
    SUB     SP, SP, #0x20
    MOV     R7, R3

    ADD     R3, SP, #0x10
    STR     R2, [SP,#0x8]
    LDR     R2, =0x8094490
    STR     R2, [R3]
    STR     R1, [R3,#4]
    STR     R3, [SP,#0x0]
    MOV     R1, #0
    STR     R1, [SP,#0x4]
    ADD     R1, SP, #0xC
    @mov     R2, #0x10
    @ldr     r2, =0xFFFF0
    MOV     R2, R7
    MOV     R3, #0@x1000
    LDR     R4, [R0]
    LDR     R4, [R4, #f_write_call]

    BLX     R4

    ADD     SP, SP, #0x20   
    LDMFD   SP!, {R4-R11,PC}
    bx	lr

