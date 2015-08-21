#ifndef __LANGUAGE_H
#define __LANGUAGE_H

#include "exename.h"

#define PROJECT_NAME  "FANS-RT ������ת������"

#define MSG_NOFOUND_NAME                                                            \
    " ���󣺲�������û���ļ��������ڲ��� %s ��ָ��Ŀ¼���ļ�����\n"                 \
    "       �������ʣ����� --help ��ð�����\n"

#define MSG_NOT_SUPPORT                                                             \
    " ���󣺲�֧�ֵĲ��� %s��\n"                                                    \
    "       �������ʣ����� --help ��ð�����\n"

#define MSG_LAWLESS_FILE                                                            \
    " ���󣺷Ƿ��ļ��� %s ���ļ����в��ܰ��� '*' '?' ��ͨ�����\n"                  \
    "       �������ʣ����� --help ��ð�����\n"

#define MSG_INVALID_COMFILE                                                         \
    " ������Ч�����ļ��� %s \n"                                                   \
    "       �������ʣ����� --help ��ð�����\n"

#define MSG_INVALID_COMRATE                                                         \
    " ������Ч�Ĵ������� %s \n"                                                   \
    "       �������ʣ����� --help ��ð�����\n"

#define MSG_FAILED_OPEN                                                             \
    " �����޷����ļ� <%s> �����ļ��Ƿ���ڡ�\n"                               \
    "       �������ʣ����� --help ��ð�����\n"
    
#define MSG_FAILED_WRITE                                                            \
    " �������ļ� <%s> д�� %ld �ֽ�ʧ�ܣ�ʵ��д�� %ld �ֽ�\n"                     \
    "       �������ʣ����� --help ��ð�����\n"

#define MSG_ELFBEGIN_FAIL                                                           \
    " ���󣺴��ļ� %s ����ELF����ʧ�ܡ�\n"

#define MSG_MEMALLOC_FAIL                                                           \
    " ���󣺷����ڴ� %d �ֽ�ʧ�ܡ�\n"

#define MSG_NULLPOINTER_FAIL                                                        \
    " ������Чָ�� %s��\n"

#define MSG_EHDRINVALID_FAIL                                                        \
    " ���󣺻�ȡELFͷʧ�ܡ�\n"

#define MSG_ELFBUFFER_FAIL                                                          \
    " ���󣺻�ȡ %s �ļ�������ʧ�ܡ�\n"

#define MSG_PHDROUTRANGE_FAIL                                                       \
    " ��������ĳ���ͷ����ֵ %d ������Χ�����ֵ %d��\n"

#define MSG_SECTION_FAIL                                                            \
    " ���󣺶����� %d ������Ҫ�� !\n"

#define MSG_DATALENGTH_FAIL                                                         \
    " ���󣺳�ʼ�����ݶγ��� %d ����Ԥ�� !\n"


#define MSG_EHDRINFOR_SHOW          "===========ELF �ļ���Ϣ===========\n"
#define MSG_FILELENGTH_SHOW         "  �ļ�����:   %d �ֽ�\n"
#define MSG_EHDRMAGIC_SHOW          "  ħ    ��:   %02X %02X %02X %02X\n"
#define MSG_EHDRETYPE_SHOW          "  ��    ��:   %s\n"
#define MSG_EHDRMACHINE_SHOW        "  �� �� ��:   %s\n"
#define MSG_EHDRVERSION_SHOW        "  ��    ��:   %d\n"
#define MSG_EHDRENTRY_SHOW          "  ��    ��:   %08X(ƫ��)\n"
#define MSG_EHDRPHDROFF_SHOW        "  ����ͷ��:   %08X(ƫ��)\n"
#define MSG_EHDRSHDROFF_SHOW        "  ��    ��:   %08X(ƫ��)\n"
#define MSG_EHDREFLAGS_SHOW         "  ��    ־:   %X\n"
#define MSG_EHDREHSIZE_SHOW         "  �� �� ͷ:   %d(����)\n"
#define MSG_EHDRPHSIZE_SHOW         "  ����ͷ��:   %d(����)\n"
#define MSG_EHDRPHNUM_SHOW          "  ����ͷ��:   %d(����)\n"
#define MSG_EHDRSHSIZE_SHOW         "  ��    ��:   %d(����)\n"
#define MSG_EHDRSHNUM_SHOW          "  ��    ��:   %d(����)\n"
#define MSG_EHDRSHSTR_SHOW          "  �ַ�����:   %d(����)\n"


#define MSG_HELP                                                                    \
    " �÷���\r\n"                                                                   \
    "     " EXENAME " -f <�����ļ�> -m <��������ļ�> -o <�������ļ�>\r\n\r\n"       \
    " ��ѡ�������£�\r\n"                                                           \
    " --help               ��ʾ������Ϣ��\r\n"                                      \
    " -f <�����ļ�>        ��Ҫת���������ļ���������һ�� ELF �ļ���\r\n"           \
    " -m <��������ļ�>    ���ڱ��� stm32 �����ļ���\r\n"                           \
    " -o <�������ļ�>      ���ɵĶ������ļ���������¼���豸�ϡ�\r\n\r\n"            \
    " ���磺 makbin.exe -f fans-rt.elf -m ld_flags.mak -o fans-rt.bin\r\n"

#endif