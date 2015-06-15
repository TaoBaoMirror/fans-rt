#ifndef __CHINESE_LANGUAGE_H
#define __CHINESE_LANGUAGE_H

#define MSG_OUTDIR          "输出目录:   %s\n"
#define MSG_OUTFILE         "输出文件:   %s.fnt  %s.msg\n"
#define MSG_INFILE          "输入文件:   %s\n"
#define MSG_CONTENT         "字符内容:   %s\n"
#define LAB_FILEINFOR                                                       \
    ";====================================================================" \
    "\x0d\x0a"                                                              \
    ";文 件 名:  %s%s\x0d\x0a"                                              \
    ";作    者:  姜勇\x0d\x0a"                                              \
    ";说    明： 本文件由工具自动生成，请勿修改。\x0d\x0a"                  \
    ";====================================================================" \
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
#define MSG_FAILED_OPEN                                                     \
    " 错误：无法打开文件 <%s> 请检查文件是否存在。\n"                       \
    "       如有疑问，请用 --help 获得帮助。\n"
#define MSG_HELP                                                            \
    " 创建 Boot sector 引导代码的中文语言字库信息\n\n"                      \
    " chtool [param] <value>\n\n"                                           \
    " 可选参数如下：\n"                                                     \
    " --help               显示帮助信息。\n"                                \
    " -m <file>            选择输入的信息文件，信息文件的内容为代码引导\n"  \
    "                      失败后的打印信息，内容中英文均可。但信息文件\n"  \
    "                      中包含的中文文字数量不得超过15个，否则将会导\n"  \
    "                      致 fat16.s/fat32.s 无法正常编译。\n"             \
    "                          默认 -p message.txt\n"                       \
    " -o <file>            选择要输出的文件名， -o 参数指定的文件名不包\n"  \
    "                      含扩展名，通常为语言名称。\n"                    \
    "                          默认 -o chinese\n"                           \
    " -p <path>            选择输出文件的路径，这里是指 .msg 和 .fnt 文\n"  \
    "                      件的输出路径。\n"                                \
    "                          默认 -p ..\\language\n"                      \
    " -font <file>         选择字体文件， chtool 只能处理编码为 GB2312 \n"  \
    "                      的 16 点阵字体文件，繁体、简体均可。\n\n"        \
    "  例如： chinese -m error.txt -p .. -o simpchin -font hzk16f\n"

#endif


