/*****************************************************************************
*文 件 名:  language.h
*作    者:  姜勇
*修    订:
*            2011-10-15            姜勇              创建文件
*版    本:  beta 0.11.10.15
*功    能:  头文件。
*编    译:  gcc -o table table.c
*****************************************************************************/
#ifndef __CHINESE_LANGUAGE_H
#define __CHINESE_LANGUAGE_H

#define MSG_LOGO                                                            \
    " 源代码跳格转换工具(v1.0)\n"                                           \
    " 作    者： 姜勇\n"
#define MSG_OUTFILE         " 备份文件： %s.bak\n"
#define MSG_INFILE          " 转换文件： %s\n"
#define MSG_TABLESIZE       " 跳格长度： %d\n"
#define MSG_INVALID_PCOUNT                                                  \
    " 错误：参数不足，至少应该输入一个参数 <文件名> ！\n"                   \
    "       如有疑问，请用 --help 获得帮助。\n"
#define MSG_BUFFER_FAILE                                                    \
    " 错误：无法为文件 %s 申请缓冲区，可能内存不足，或文件长度为 0！\n"     \
    "       如有疑问，请用 --help 获得帮助。\n"
#define MSG_INVALID_SIZE                                                    \
    " 错误：跳格符的长度 %d 不正确，4 或 8 才是正确的选择范围。\n"          \
    "       如有疑问，请用 --help 获得帮助。\n"
#define MSG_NOFOUND_FILE                                                    \
    " 错误：没有文件需要转换？请输入你要转换的文件！\n"                     \
    "       如有疑问，请用 --help 获得帮助。\n"
#define MSG_NOFOUND_NAME                                                    \
    " 错误：参数后面没有文件名，请在参数 %s 后指定目录或文件名。\n"         \
    "       如有疑问，请用 --help 获得帮助。\n"
#define MSG_LAWLESS_FILE                                                    \
    " 错误：非法文件名 %s ，文件名中不能包含 '*' '?' 等通配符。\n"          \
    "       如有疑问，请用 --help 获得帮助。\n"
#define MSG_INVALID_PARAM                                                   \
    " 错误：无效的参数 %s , 或者没有为参数赋值。\n"                         \
    "       如有疑问，请用 --help 获得帮助。\n"
#define MSG_FAILED_OPEN                                                     \
    " 错误：无法打开文件 <%s> 请检查文件是否存在。\n"                       \
    "       如有疑问，请用 --help 获得帮助。\n"
#define MSG_HELP                                                            \
    MSG_LOGO                                                                \
    " 使用用法： table file -t <size>\n\n"                                  \
    " 可选参数如下：\n"                                                     \
    " --help           显示帮助信息。\n"                                    \
    " -t <size>        选择输入文件的跳格键长度，可选 4 和 8，其他无效。\n" \
    "                  该参数默认长度为 4。\n\n"                            \
    "  例如： table fat12.s -t 8\n"

#endif


