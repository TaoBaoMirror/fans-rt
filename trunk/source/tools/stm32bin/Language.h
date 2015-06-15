#ifndef __LANGUAGE_H
#define __LANGUAGE_H

#include "exename.h"

#define PROJECT_NAME  "FANS-RT 二进制转换工具"

#define MSG_NOFOUND_NAME                                                            \
    " 错误：参数后面没有文件名，请在参数 %s 后指定目录或文件名。\n"                 \
    "       如有疑问，请用 --help 获得帮助。\n"

#define MSG_NOT_SUPPORT                                                             \
    " 错误：不支持的参数 %s。\n"                                                    \
    "       如有疑问，请用 --help 获得帮助。\n"

#define MSG_LAWLESS_FILE                                                            \
    " 错误：非法文件名 %s ，文件名中不能包含 '*' '?' 等通配符。\n"                  \
    "       如有疑问，请用 --help 获得帮助。\n"

#define MSG_INVALID_COMFILE                                                         \
    " 错误：无效串口文件名 %s \n"                                                   \
    "       如有疑问，请用 --help 获得帮助。\n"

#define MSG_INVALID_COMRATE                                                         \
    " 错误：无效的串口速率 %s \n"                                                   \
    "       如有疑问，请用 --help 获得帮助。\n"

#define MSG_FAILED_OPEN                                                             \
    " 错误：无法打开文件 <%s> 请检查文件是否存在。\n"                               \
    "       如有疑问，请用 --help 获得帮助。\n"
    
#define MSG_FAILED_WRITE                                                            \
    " 错误：向文件 <%s> 写入 %ld 字节失败，实际写入 %ld 字节\n"                     \
    "       如有疑问，请用 --help 获得帮助。\n"

#define MSG_ELFBEGIN_FAIL                                                           \
    " 错误：从文件 %s 创建ELF对象失败。\n"

#define MSG_MEMALLOC_FAIL                                                           \
    " 错误：分配内存 %d 字节失败。\n"

#define MSG_NULLPOINTER_FAIL                                                        \
    " 错误：无效指针 %s。\n"

#define MSG_EHDRINVALID_FAIL                                                        \
    " 错误：获取ELF头失败。\n"

#define MSG_ELFBUFFER_FAIL                                                          \
    " 错误：获取 %s 文件缓冲区失败。\n"

#define MSG_PHDROUTRANGE_FAIL                                                       \
    " 错误：输入的程序头索引值 %d 超出范围，最大值 %d。\n"

#define MSG_SECTION_FAIL                                                            \
    " 错误：段数量 %d 不符合要求 !\n"

#define MSG_DATALENGTH_FAIL                                                         \
    " 错误：初始化数据段长度 %d 超出预期 !\n"


#define MSG_EHDRINFOR_SHOW          "===========ELF 文件信息===========\n"
#define MSG_FILELENGTH_SHOW         "  文件长度:   %d 字节\n"
#define MSG_EHDRMAGIC_SHOW          "  魔    数:   %02X %02X %02X %02X\n"
#define MSG_EHDRETYPE_SHOW          "  类    型:   %s\n"
#define MSG_EHDRMACHINE_SHOW        "  处 理 器:   %s\n"
#define MSG_EHDRVERSION_SHOW        "  版    本:   %d\n"
#define MSG_EHDRENTRY_SHOW          "  入    口:   %08X(偏移)\n"
#define MSG_EHDRPHDROFF_SHOW        "  程序头表:   %08X(偏移)\n"
#define MSG_EHDRSHDROFF_SHOW        "  段    表:   %08X(偏移)\n"
#define MSG_EHDREFLAGS_SHOW         "  标    志:   %X\n"
#define MSG_EHDREHSIZE_SHOW         "  文 件 头:   %d(长度)\n"
#define MSG_EHDRPHSIZE_SHOW         "  程序头表:   %d(长度)\n"
#define MSG_EHDRPHNUM_SHOW          "  程序头表:   %d(数量)\n"
#define MSG_EHDRSHSIZE_SHOW         "  段    表:   %d(长度)\n"
#define MSG_EHDRSHNUM_SHOW          "  段    表:   %d(数量)\n"
#define MSG_EHDRSHSTR_SHOW          "  字符串表:   %d(索引)\n"


#define MSG_HELP                                                                    \
    " 用法：\r\n"                                                                   \
    "     " EXENAME " -f <启动文件> -m <编译参数文件> -o <二进制文件>\r\n\r\n"       \
    " 可选参数如下：\r\n"                                                           \
    " --help               显示帮助信息。\r\n"                                      \
    " -f <启动文件>        需要转换的启动文件，必须是一个 ELF 文件。\r\n"           \
    " -m <编译参数文件>    用于编译 stm32 启动文件。\r\n"                           \
    " -o <二进制文件>      生成的二进制文件，可以烧录到设备上。\r\n\r\n"            \
    " 例如： makbin.exe -f fans-rt.elf -m ld_flags.mak -o fans-rt.bin\r\n"

#endif