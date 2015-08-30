#ifndef __CHINESE_LANGUAGE_H
#define __CHINESE_LANGUAGE_H

#define MSG_OUT_OF_MEMORY                                                   \
    " 错误：内存空间不足，无法执行内存分配。"
#define MSG_NOFOUND_NAME                                                    \
    " 错误：参数后面没有文件名，请在参数 %s 后指定目录或文件名。\n"         \
    "       如有疑问，请用 --help 获得帮助。\n"
#define MSG_LAWLESS_FILE                                                    \
    " 错误：非法文件名 %s ，文件名中不能包含 '*' '?' 等通配符。\n"          \
    "       如有疑问，请用 --help 获得帮助。\n"
#define MSG_INVALID_PARAM                                                   \
    " 错误：指定的选项 [%s] 不能被支持，请使用正确的选项。\n"               \
    "       请用 --help 获得更多帮助信息。\n"
#define MSG_INVALID_VALUE                                                   \
    " 错误：选项 [%s] 的参数 %s 必须不是一个有效数字。\n"                   \
    "       请用 --help 获得更多帮助信息。\n"
#define MSG_INVALID_COUNT                                                   \
    " 错误：没有为选项 [%s] 指定输入参数。\n"                               \
    "       请用 --help 获得更多帮助信息。\n"
#define MSG_FAILED_OPEN                                                     \
    " 错误：无法打开文件 <%s> 请检查文件是否存在。\n"                       \
    "       请用 --help 获得更多帮助信息。\n"

#define PREFIX_HELP                                                         \
    " fans-rt 虚拟磁盘加载工具\n\n"                                         \
    " 用法: install [-u] [-r] <routine> [-i] <file> [-d] <driver> ... \n\n" \
    " 可选参数：\n"

#define TOOL_NAME                                                           \
    " fans-rt 虚拟磁盘加载工具"
#define USAGE_HELP                                                          \
    " 用法: install [-u] [-r] <routine> [-i] <file> [-d] <driver> ..."
#define NOTES_HELP                                                          \
    " 可选参数："
#define EXAMPLE_HELP                                                        \
    "\n 例如： install -r c:\\windows\\vdisk.sys -i fans-rt.img -d A\n"

#define COMMENT1_HELP           " "
#define COMMENT2_HELP           "显示帮助信息"
#define COMMENT1_ROUTINE        "<routine>"
#define COMMENT2_ROUTINE                                                    \
     "选择驱动程序的文件路径。"
#define COMMENT1_IMAGE          "<image>"
#define COMMENT2_IMAGE                                                      \
    "指定用于虚拟磁盘的镜像文件。"
#define COMMENT1_DRIVER         "<driver>"
#define COMMENT2_DRIVER                                                     \
    "指定需要卸载或创建的虚拟驱动器盘符，例如 -d A 则创建一个盘符为 A 的"   \
    "驱动器。\n注意：在创建虚拟驱动器时，指定的目标驱动器盘符必须是一个未"  \
    "被系统使用的盘符，否则将导致创建失败。"
#define COMMENT1_UNSTALL        " "
#define COMMENT2_UNSTALL                                                    \
    "卸载指定的虚拟驱动器盘符，卸载后虚拟驱动器将不可用。需要卸载的盘符"    \
    "通过参数 [-d] 指定。如果 [-u] 没有被指定，则表示创建一个新的磁盘。"

#endif


