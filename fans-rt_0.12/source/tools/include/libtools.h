/*
 *    Copyright(C) 2013-2015, Fans-rt development team.
 *
 *    All rights reserved.
 *
 *    This is open source software.
 *    Learning and research can be unrestricted to modification, use and dissemination.
 *    If you need for commercial purposes, you should get the author's permission.
 *
 *    date           author          notes
 *    2015-08-30     JiangYong       new file
 */
#ifndef __LIB_TOOLS_H
#define __LIB_TOOLS_H

#define     INVALID_READ        -1
#define     INVALID_FILE        -2
#define     INVALID_PARAM       -3
#define     INVALID_VALUE       -4
#define     INVALID_COUNT       -5
#define     INVALID_FOUND       -6
#define     INVALID_MEMORY      -7
#define     INTERNAL_ERROR      -8
#define     INVALID_SIZE        -9

#ifndef     FAR
#define     FAR
#endif

#ifndef     FALSE
#define     TRUE        1
#define     FALSE       0
#endif

#ifdef      _GNUC_
#define     ATTRIBUTE_PACKED        __attribute__ ((packed))
#define     ATTRIBUTE_ALIGN_1       __attribute__ ((__aligned__ (1)))
#else
#define     ATTRIBUTE_PACKED
#define     ATTRIBUTE_ALIGN_1
#endif

#ifndef     WINVER
typedef     unsigned char       BOOL __attribute__ ((__aligned__ (1)));
typedef     unsigned char       BYTE __attribute__ ((__aligned__ (1)));
typedef     unsigned short      WORD __attribute__ ((__aligned__ (1)));
typedef     unsigned int        DWORD __attribute__ ((__aligned__ (1)));
#else
typedef     unsigned char       BYTE;
typedef     unsigned short      WORD;
#endif

#define     SIZEOF_ARRAY(x)     (sizeof(x)/sizeof(x[0]))

typedef struct _t_tool_descriptor       tool_descriptor;
typedef struct _t_parameter_descriptor  parameter_descriptor;

enum{
    DT_NULL     =   0,
    DT_HELP     =   1,
    DT_FLAG     =   2,
    DT_FILE     =   3,
    DT_NUMBER   =   4,
    DT_STRING   =   5,
    DT_MAX,
};

struct _t_tool_descriptor
{
    char *                  name;               /**< the tool name */
    char *                  path;               /**< the tool path */
    char *                  note;               /**< A brief description of the functions of the tool */
    char *                  usage;              /**< Usage of this tool */
    char *                  prompt;             /**< The prompt information of the argument description */
    char *                  example;            /**< The example command line */
    unsigned int            width;
    unsigned int            seek;
    unsigned int            total;              /**< The argument count */
    parameter_descriptor *  arguments;          /**< The arguments descriptor table */
    void *                  tprivate;           /**< Pointer to the private date */
};

struct _t_parameter_descriptor
{    
    char *                  option;             /**< The command line option string */
    unsigned int            dtype;              /**< The type of this option */
    int                     value;              /**< If this is a digital option, this member is valid */
    char *                  string;             /**< If this is a string option, this member is valid */
    const char *            comment1;           /**< The member describes the parameters of the option */
    const char *            comment2;           /**< Details of the option */
    void *                  buffer;             /**< The buffer of this option in tool object */
    size_t                  length;             /**< The buffer length */
};


#define TOOLS_ASSERT(condition, execute, ...)                                                   \
        do {                                                                                    \
            if (!(condition)){                                                                  \
                printf("[%04d][%s]:", __LINE__, __FUNCTION__);                                  \
                printf(__VA_ARGS__);                                                            \
                printf("\n");                                                                   \
                execute;                                                                        \
            }                                                                                   \
        } while(0)

#define TOOLS_LOG(...)                                                                          \
        do {                                                                                    \
            printf("[%04d][%s]:", __LINE__, __FUNCTION__);                                      \
            printf(__VA_ARGS__);                                                                \
            printf("\n");                                                                       \
        } while(0)

#ifdef __cplusplus
extern "C" {
#endif
    int tool_scan_argument(int argc,char * argv[], tool_descriptor * descriptor);
    int tool_run(tool_descriptor * descriptor);
#ifdef __cplusplus
}
#endif  

#endif
