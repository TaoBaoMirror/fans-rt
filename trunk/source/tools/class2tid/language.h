#ifndef __CHINESE_LANGUAGE_H
#define __CHINESE_LANGUAGE_H

#define MSG_OUTFILE         "输出文件:   %s\n"
#define MSG_INFILE          "输入文件:   %s\n"

#define LAB_FILEINFOR                                                       \
    "/********************************************************************" \
    "\x0d\x0a"                                                              \
    ";文 件 名:  %s\x0d\x0a"                                                \
    ";作    者:  姜勇\x0d\x0a"                                              \
    ";说    明： 本文件由工具自动生成，请勿修改。\x0d\x0a"                  \
    "********************************************************************/" \
    "\x0d\x0a"
    
#define MSG_NOFOUND_NAME                                                    \
    " 错误：参数后面没有文件名，请在参数 %s 后指定目录或文件名。\n"         \
    "       如有疑问，请用 --help 获得帮助。\n"
#define MSG_LAWLESS_FILE                                                    \
    " 错误：非法文件名 %s ，文件名中不能包含 '*' '?' 等通配符。\n"          \
    "       如有疑问，请用 --help 获得帮助。\n"
#define MSG_INVALID_PARAM                                                   \
    " 错误：无效的参数 %s , 或者没有为参数赋值。\n"                         \
    "       如有疑问，请用 --help 获得帮助。\n"
#define MSG_INVALID_FILESIZE                                                \
    " 错误：文件 <%s> 长度 %d 字节无效。\n"                                 \
    "       如有疑问，请用 --help 获得帮助。\n"
#define MSG_FAILED_OPEN                                                     \
    " 错误：无法打开文件 <%s> 请检查文件是否存在。\n"                       \
    "       如有疑问，请用 --help 获得帮助。\n"
#define MSG_TABLECREATE_FAILED                                              \
    " 错误：文件 <%s> 长度 %d 字节无效。\n"                                 \
    "       如有疑问，请用 --help 获得帮助。\n"
    
#define MSG_HELP                                                            \
    " 对象长度查询表生成工具(v1.0)\n"                                       \
    " 作者： 姜勇\n"                                                        \
    " 使用方法： class2tid -c <config> -o <table>\n\n"                      \
    " 可选参数如下：\n"                                                     \
    " --help           显示帮助信息。\n"                                    \
    " -c <config>      选择输入的配置文件，一般为 path\\config.h。\n"       \
    " -o <table>       选择输出的c代码文件。\n\n"                           \
    "  例如： class2tid -c config.h -o classtable.c\n"                      \

#endif


