#ifndef __LIB_TOOLS_H
#define __LIB_TOOLS_H

#define     NO_FILE             -1
#define     INVALID_FILE        -2
#define     INVALID_PARAM       -3
#define     INVALID_VALUE       -4
#define     INVALID_COUNT       -5
#define     NOT_FOUND           -6
#define     INVALID_MEMORY      -7

#define     SIZEOF_ARRAY(x)     (sizeof(x)/sizeof(x[0]))

typedef struct _t_tool_descriptor       tool_descriptor;
typedef struct _t_parameter_descriptor  parameter_descriptor;

typedef int (*fncheck)(int argc, char * argv[], int who, parameter_descriptor * descriptor);

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
    char *                  name;
    char *                  usage;
    char *                  example;
    unsigned int            total;
    parameter_descriptor *  arguments;
    void *                  tprivate;
};

struct _t_parameter_descriptor
{    
    char *                  label;
    unsigned int            dtype;
    int                     value;
    char *                  string;
    const char *            comment1;
    const char *            comment2;
};

#define tools_assert(condition, execute, ...)                                                   \
        do {                                                                                    \
            if (!(condition)){                                                                  \
                printf("[%04d][%s]:", __LINE__, __FUNCTION__);                                  \
                printf(__VA_ARGS__);                                                            \
                execute;                                                                        \
            }                                                                                   \
        } while(0)

#ifdef __cplusplus
extern "C" {
#endif
    int descriptor_get_number(tool_descriptor * descriptor, char * label, int * value);
    int descriptor_get_string(tool_descriptor * descriptor, char * label, char * * string);
    int descriptor_scan_argument(int argc,char * argv[], tool_descriptor * descriptor);
    int descriptor_show_help(tool_descriptor * descriptor, size_t line, size_t skip);
#ifdef __cplusplus
}
#endif  

#endif
