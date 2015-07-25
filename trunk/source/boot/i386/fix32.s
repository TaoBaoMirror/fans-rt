BPB_OFFSET      EQU     0x0003
OLD_OFFSET      EQU     0x0800
NEW_OFFSET      EQU     0x0200
BPB_LENGTH      EQU     0x0057

org 0x100
entry:
        mov     ax,     0x0201
        mov     bx,     OLD_OFFSET
        mov     cx,     0x0001
        mov     dx,     0x0180
        int     0x13
        jc      .@9
        db      0xcc
        mov     si,     OLD_OFFSET
        mov     di,     NEW_OFFSET
        add     si,     BPB_OFFSET
        add     di,     BPB_OFFSET
        mov     cx,     BPB_LENGTH
        cld
        rep     movsb
        db      0xcc
        mov     ecx,    3
        mov     eax,    1
        mov     di,     (NEW_OFFSET+0x200-0x8)
.@1:
        pushf
        std
        stosd
        popf
        inc     eax
        loop    .@1
.@2:
        mov     ax,     0x0301
        mov     bx,     NEW_OFFSET
        mov     cx,     0x0001
        mov     dx,     0x0180
        int     0x13
        jc      .@2
.@3:
        mov     ax,     0x301
        mov     bx,     NEW_OFFSET+0x0200
        mov     cx,     2
        mov     dx,     0x0080
        int     0x13
        jc      .@3
.@4:
        mov     ax,     0x301
        mov     bx,     NEW_OFFSET+0x0400
        mov     cx,     3
        mov     dx,     0x0080
        int     0x13
        jc      .@4
.@5:
        mov     ax,     0x301
        mov     bx,     NEW_OFFSET+0x0600
        mov     cx,     4
        mov     dx,     0x0080
        int     0x13
        jc      .@5
        ret
.@9:
        mov     si,     @MESSAGE
        call    @PRINTF
        ret
;=========================================================================================================================
;@PRINTF        打印字符串
;入口：         si              =               目标字符串指针
;出口：         无
;说明：
;               该子过程会执行后会修改 ds/ax/bx 寄存器值，其他寄存器不受影响.
;=========================================================================================================================
@PRINTF:
        push    cs
        pop     ds
.@1:
        lodsb                                                   ; 从 ds:si中读入一个字符到 al
        or      al,     al                                      ; 检测字符是否为0
        jz      .@2                                             ; 显示完毕, 退出过程
        mov     ah,     0x0e                                    ; 显示字符的 BIOS INT10H 请求号
        mov     bl,     0x09                                    ; 
        int     0x10
        jmp     short   .@1
.@2:
        ret
@MESSAGE:       db      "ERROR: read disk failed !",0x0d,0x0a,0
times   NEW_OFFSET-0x100-($-$$) db      0
incbin "fat32.bin"