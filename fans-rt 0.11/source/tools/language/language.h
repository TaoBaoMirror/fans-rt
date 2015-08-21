#ifndef __CHINESE_LANGUAGE_H
#define __CHINESE_LANGUAGE_H

#define MSG_OUTDIR          "���Ŀ¼:   %s\n"
#define MSG_OUTFILE         "����ļ�:   %s.fnt  %s.msg\n"
#define MSG_INFILE          "�����ļ�:   %s\n"
#define MSG_CONTENT         "�ַ�����:   %s\n"
#define LAB_FILEINFOR                                                       \
    ";====================================================================" \
    "\x0d\x0a"                                                              \
    ";�� �� ��:  %s%s\x0d\x0a"                                              \
    ";��    ��:  ����\x0d\x0a"                                              \
    ";˵    ���� ���ļ��ɹ����Զ����ɣ������޸ġ�\x0d\x0a"                  \
    ";====================================================================" \
    "\x0d\x0a"
    
#define MSG_NOFOUND_NAME                                                    \
    " ���󣺲�������û���ļ��������ڲ��� %s ��ָ��Ŀ¼���ļ�����\n"         \
    "       �������ʣ����� --help ��ð�����\n"
#define MSG_LAWLESS_FILE                                                    \
    " ���󣺷Ƿ��ļ��� %s ���ļ����в��ܰ��� '*' '?' ��ͨ�����\n"          \
    "       �������ʣ����� --help ��ð�����\n"
#define MSG_INVALID_PARAM                                                   \
    " ������Ч�Ĳ��� %s , ����û��Ϊ������ֵ��\n"                         \
    "       �������ʣ����� --help ��ð�����\n"
#define MSG_FAILED_OPEN                                                     \
    " �����޷����ļ� <%s> �����ļ��Ƿ���ڡ�\n"                       \
    "       �������ʣ����� --help ��ð�����\n"
#define MSG_HELP                                                            \
    " ���� Boot sector ������������������ֿ���Ϣ\n\n"                      \
    " chtool [param] <value>\n\n"                                           \
    " ��ѡ�������£�\n"                                                     \
    " --help               ��ʾ������Ϣ��\n"                                \
    " -m <file>            ѡ���������Ϣ�ļ�����Ϣ�ļ�������Ϊ��������\n"  \
    "                      ʧ�ܺ�Ĵ�ӡ��Ϣ��������Ӣ�ľ��ɡ�����Ϣ�ļ�\n"  \
    "                      �а��������������������ó���15�������򽫻ᵼ\n"  \
    "                      �� fat16.s/fat32.s �޷��������롣\n"             \
    "                          Ĭ�� -p message.txt\n"                       \
    " -o <file>            ѡ��Ҫ������ļ����� -o ����ָ�����ļ�������\n"  \
    "                      ����չ����ͨ��Ϊ�������ơ�\n"                    \
    "                          Ĭ�� -o chinese\n"                           \
    " -p <path>            ѡ������ļ���·����������ָ .msg �� .fnt ��\n"  \
    "                      �������·����\n"                                \
    "                          Ĭ�� -p ..\\language\n"                      \
    " -font <file>         ѡ�������ļ��� chtool ֻ�ܴ������Ϊ GB2312 \n"  \
    "                      �� 16 ���������ļ������塢������ɡ�\n\n"        \
    "  ���磺 chinese -m error.txt -p .. -o simpchin -font hzk16f\n"

#endif


