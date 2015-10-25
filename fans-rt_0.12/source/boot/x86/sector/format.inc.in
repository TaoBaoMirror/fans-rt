;=========================================================================================================================
;文 件 名:      format.inc
;作    者:      姜勇
;创建日期:      2005-11-31
;修    订:
;               2011-10-12            姜勇           创建文件
;               2015-08-23            姜勇           修改为format.inc, 适配编译规则
;版    本:      beta 0.11.10.9
;修改日期:      2015.08.23
;功    能:      处理不同的 loader 文件格式，可支持 MSDOS 的 MZEXE 格式，ELF格式和BIN格式
;编    译:      nasm -fbin -o fat16.bin fat16.s
;说    明:
;=========================================================================================================================
%include        "boot.inc"
%ifndef LOAD_FORMAT
%define LOAD_FORMAT             ELFLOADER
%endif
;=========================================================================================================================
;@FIND_ENTRY    查找LOADER的入口地址
;入口：         无
;出口：
;               cf              =               0       成功
;               cf              =               1       失败
;说明：
;=========================================================================================================================
@FIND_ENTRY:
%if     LOAD_FORMAT=BINLOADER
        mov     ax,     LOADER_SEGBASE                          ; ax = Segment of file <loader>
        shl     eax,    16                                      ; eax = Segment:offset
        ret
%elif   LOAD_FORMAT=MZLOADER
        mov     ax,     LOADER_SEGBASE                          ; ax = Segment of file <loader>
        mov     ds,     ax                                      ; ds = Segment of file <loader>
        mov     si,     OFFSET_DOSHDR_SIZE                      ; 记录 DOS MZ 文件头大小的成员的偏移地址
        add     ax,     [si+0]                                  ; 获得 DOS MZ 文件头大小
        mov     dl,     [@DRIVER_ID_VALUE]
        shl     eax,    16                                      ; eax = Segment:offset
        ret
%elif   LOAD_FORMAT=ELFLOADER
        mov     eax,    LOADER_SEGBASE                          ; ax  =  LOADER 加载的段地址
        mov     ds,     ax
        shl     eax,    4
        add     eax,    [ELFHDR_OFFSET_SHOFF]
        mov     cx,     [ELFHDR_OFFSET_SHNUM]
        mov     si,     ax
        and     si,     0x000f
        shr     eax,    4
        mov     ds,     ax
.@1:
        mov     ebx,    [si+SECHDR_OFFSET_OFFSET]
        mov     eax,    [si+SECHDR_OFFSET_MMADDR]
        add     si,     SIZEOF_SECHDR
        cmp     eax,    PROTAL_SEGBASE
        jz      .@2
        loop    .@1
        stc
        jc      .@9
.@2:
        add     ebx,    (LOADER_SEGBASE<<4)
        movzx   eax,    bx
        shr     ebx,    4
        and     ax,     0x000f
        shl     ebx,    16
        add     eax,    ebx
.@9:
        ret
%endif
