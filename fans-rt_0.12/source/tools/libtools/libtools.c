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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

#include "libtools.h"
#include "language.h"

#if 0
/**************************************************************************
 * Check the argument and set help flag.
 * param: The argument count from main() function.
 * param: The argument value array from main() function.
 * param: The index for argument value array.
 * param: The parameter descriptor struct pointer.
 * return: If return 0 mean successfully, the other value means failure.
 *
 *     If this argument is '-h', '-H', '--h', '--H', '/?', '-?', '--help',
 * we will be set the value member of descriptor to 1. we can check the help
 * value to determine show the help messages.
 *
 * date           author          notes
 * 2015-08-30     JiangYong       new function
 **************************************************************************/
static int argument_set_help(int argc, char * argv[], int who,
                             parameter_descriptor * descriptor)
{
    int i;
    char * labels[] = {
        "-h", "-H", "--h", "--H", "/?", "-?",
        "--help", "-help", "--HELP", "-HELP",
    };

    for(i = 0; i < SIZEOF_ARRAY(labels); i++)
    {
        if(0 == strcmp(argv[who], labels[i]))
        {
            descriptor->value = 1;
            return 0;
        }
    }

    return INVALID_PARAM;
}
#endif
/**************************************************************************
 * Check the argument and set the string menber of descriptor.
 * param: The argument count from main() function.
 * param: The argument value array from main() function.
 * param: The index for argument value array.
 * param: The parameter descriptor struct pointer.
 * return: If return 0 mean successfully, the other value means failure.
 *
 *     If this argument argv[who] equal to the label menber of this descriptor,
 * we will be set the string member of descriptor to argv[who + 1].
 *
 * date           author          notes
 * 2015-08-30     JiangYong       new function
**************************************************************************/
static int argument_set_string(int argc, char * argv[], int who,
                                      parameter_descriptor * descriptor)
{
    if (0 != strcmp(descriptor->option, argv[who]))
    {
        return INVALID_PARAM;
    }
    
    if (who + 1 >= argc)
    {
        printf(MSG_INVALID_COUNT, argv[who]);
        return INVALID_COUNT;
    }
    
    descriptor->string = argv[who + 1];
    
    return 1;
}

/**************************************************************************
 * Check the argument and set the value menber of descriptor.
 * param: The argument count from main() function.
 * param: The argument value array from main() function.
 * param: The index for argument value array.
 * param: The parameter descriptor struct pointer.
 * return If return 0 mean successfully, the other value means failure.
 *
 *     If this argument argv[who] equal to the label menber of this descriptor,
 * we will be set the value member of descriptor to argv[who + 1].
 *
 * date           author          notes
 * 2015-08-30     JiangYong       new function
**************************************************************************/
static int argument_set_value(int argc, char * argv[], int who,
                                     parameter_descriptor * descriptor)
{
    char * next = NULL;

    if (0 != strcmp(descriptor->option, argv[who]))
    {
        return INVALID_PARAM;
    }
    
    if (who + 1 >= argc)
    {
        printf(MSG_INVALID_COUNT, argv[who]);
        return INVALID_COUNT;
    }

    descriptor->value = strtol(argv[who + 1], &next, 0);
    
    if (next == argv[who + 1])
    {
        printf(MSG_INVALID_VALUE, argv[who], argv[who + 1]);
        return INVALID_VALUE;
    }
    
    return 1;
}

/**************************************************************************
 * Check the argument and set the value menber of descriptor.
 * param: The argument count from main() function.
 * param: The argument value array from main() function.
 * param: The index for argument value array.
 * param: The parameter descriptor struct pointer.
 * return If return 0 mean successfully, the other value means failure.
 *
 *     If this argument argv[who] equal to the label menber of this descriptor,
 * we will be set the value member of descriptor to 1.
 *
 * date           author          notes
 * 2015-08-30     JiangYong       new function
**************************************************************************/
static int argument_set_status(int argc, char * argv[], int who,
                                      parameter_descriptor * descriptor)
{
    if (0 != strcmp(descriptor->option, argv[who]))
    {
        return INVALID_PARAM;
    }

    descriptor->value = 1;

    return 0;
}

/**************************************************************************
 * Search the argument and return the string member of descriptor
 * param: The parameter descriptor struct array.
 * param: The number of descriptor struct array;
 * param: The label want to serach.
 * param: The string will be return.
 * return If return 0 mean successfully, the other value means failure.
 *
 * date           author          notes
 * 2015-08-30     JiangYong       new function
**************************************************************************/
static int argument_get_string(parameter_descriptor descriptor[], int count,
                                     char * option, char * * string)
{
    int i;
    
    TOOLS_ASSERT(count > 0, return -1, "BUG: count(%d) is invalid.", count);
    TOOLS_ASSERT(NULL != string, return -1, "BUG: string is NULL.");
    
    
    for (i = 0; i < count; i ++)
    {
        if (descriptor[i].option == option)
        {
            if (DT_FILE != descriptor[i].dtype &&
                DT_STRING != descriptor[i].dtype)
            {
                TOOLS_LOG("[%s] :  invalid table config.", descriptor[i].option);
                printf(MSG_FATAL_ERROR);
                return INTERNAL_ERROR;
            }
            
            if (NULL != descriptor[i].string)
            {
                *string = descriptor[i].string;
                return 0;
            }
            
            return INVALID_VALUE;
        }
    }
    
    return INVALID_FOUND;
}

/**************************************************************************
 * Search the argument and return the value member of descriptor.
 * param: The tool descriptor struct.
 * param: The label want to serach.
 * param: The value will be return.
 * return If return 0 mean successfully, the other value means failure.
 *
 * date           author          notes
 * 2015-08-30     JiangYong       new function
 **************************************************************************/
static int descriptor_get_string(tool_descriptor * descriptor,
                                       char * option, char * * string)
{
    return argument_get_string(descriptor->arguments, descriptor->total, option, string);
}

/**************************************************************************
 * Search the argument and return the value member of descriptor.
 * param: The parameter descriptor struct array.
 * param: The number of descriptor struct array;
 * param: The label want to serach.
 * param: The value will be return.
 * return If return 0 mean successfully, the other value means failure.
 *
 * date           author          notes
 * 2015-08-30     JiangYong       new function
 **************************************************************************/
static int argument_get_value(parameter_descriptor descriptor[],
                                     int count, char * option, int * value)
{
    int i;

    TOOLS_ASSERT(option, return INVALID_PARAM, "BUG: option is NULL.");
    TOOLS_ASSERT(value, return INVALID_PARAM, "BUG: value is NULL.");
    
    for (i = 0; i < count; i ++)
    {
        if (descriptor[i].option == option)
        {
            if (DT_HELP != descriptor[i].dtype &&
                DT_FLAG != descriptor[i].dtype &&
                DT_NUMBER != descriptor[i].dtype)
            {
                TOOLS_LOG("[%s] :  invalid table config.", descriptor[i].option);
                printf(MSG_FATAL_ERROR);
                return INTERNAL_ERROR;
            }

            *value = descriptor[i].value;
            return 0;
        }
    }

    return INVALID_FOUND;
}

/**************************************************************************
 * Search the argument and return the value member of descriptor.
 * param: The tool descriptor struct.
 * param: The label want to serach.
 * param: The value will be return.
 * return If return 0 mean successfully, the other value means failure.
 *
 * date           author          notes
 * 2015-08-30     JiangYong       new function
 **************************************************************************/
static int descriptor_get_number(tool_descriptor * descriptor,
                                         char * option, int * value)
{
    return argument_get_value(descriptor->arguments, descriptor->total, option, value);
}

/**************************************************************************
 * Put string to buffer.
 * param: The out put buffer.
 * param: The source string;
 * param: The size of out put buffer.
 * param: The split symbol(use to fill buffer).
 * return: If return 0 mean successfully, the other value means failure.
 *
 * date           author          notes
 * 2015-08-30     JiangYong       new function
 **************************************************************************/
static size_t string_out(signed char * buffer, const signed char * source,
                         size_t length, char split, size_t count)
{
    int flag = 0;
    size_t i, j;
    
    for (i = 0; i < length; i ++)
    {
        if (!source[i]) break;
        else if ('\n' == source[i])
        {
            i ++;  /* drop '\n' */
            break;
        }
        /* It's chinese char. */
        else if (source[i] > 0) flag = 0;
        else flag = flag ? 0 : 1;
        
        buffer[i] = source[i];
    }

    /* Drop the last chinese char at the end of current line. */
    /* The last chinese char will be put to next line. */
    if (i == length && 1 == flag)
    {
        buffer[--i] = '\0';    
    }

    /* Fill the split char to the end of current line. */
    for (j = 0; j < count && i < length; j++, i++)
    {
        buffer[i] = split;
    }
    
    return i;
}

/**************************************************************************
 * Request output message in paragraph format.
 * param: The message string.
 * param: The maximum length of a message line.
 * param: The first character of a line.
 * param: The first character in front of the fill symbol
 * return: If return 0 mean successfully, the other value means failure.
 *
 * date           author          notes
 * 2015-08-30     JiangYong       new function
**************************************************************************/
static int show_seek_line(char * string, size_t line, int seek, char symbol)
{
    size_t offset = 0;
    char * buffer = malloc(line);

    if (NULL == buffer)
    {
        printf("%s", MSG_OUT_OF_MEMORY);
        return INVALID_MEMORY;
    }

    /* Set the skip chars to symbol.*/
    memset(buffer, symbol, seek);

    while(offset < line && string[offset])
    {
        memset(buffer + seek, 0, line - seek);
        /* Put the remain chars of comment2. */
        offset += string_out(buffer + seek, string + offset,
            line - seek - 1, symbol, 0);
        printf("%s\n", buffer, offset);
    }

    putchar('\n');

    free(buffer);

    return 0;
}

/**************************************************************************
 * Show the help messages
 * param: The parameter descriptor struct array.
 * param: The number of descriptor struct array;
 * param: The char numbers of every row.
 * param: The char numbers to skip of every row.
 * return: If return 0 mean successfully, the other value means failure.
 *
 * date           author          notes
 * 2015-08-30     JiangYong       new function
**************************************************************************/
static int argument_show_help(parameter_descriptor descriptor[],
                                       int count, size_t line, size_t seek)
{
    int i;
    char * buffer;
    size_t length;
    size_t scale = 0;
    size_t offset = 0;
    
    buffer = malloc(line);
    
    if (NULL == buffer)
    {
        printf("%s", MSG_OUT_OF_MEMORY);
        return INVALID_MEMORY;
    }
    
    for (i = 0; i < count; i ++)
    {
        memset(buffer, 0, line);

        /* The snprintf() function for MinGW has a bug, so use string_out() */
        /* Put 2 space char to home */
        scale = string_out(buffer, "  ", line, ' ', 0);
        /* Put the option member of descriptor struct */
        scale += string_out(buffer + scale, descriptor[i].option, line, ' ', 1);
        /* Put the comment1 member of descriptor struct */
        scale += string_out(buffer + scale, descriptor[i].comment1, line, ' ', 1);

        length = strlen(descriptor[i].comment2);

        if (scale >= seek)
        {
            /* If scale out of skip, put the buffer to screen and the */
            /* comment2 will be put to next row.*/
            printf("%s\n", buffer);
        }
        else
        {
            /* If scale not out of skip, the comment2 will */
            /* be put to the end of current row.*/
            memset(buffer + scale, ' ', seek - scale);
            offset = string_out(buffer + seek, descriptor[i].comment2,
                                line - seek - 1, ' ', 0);
            printf("%s\n", buffer);
        }

        /* Set the skip chars to space.*/
        memset(buffer, ' ', seek);

        while(offset < length && descriptor[i].comment2[offset])
        {
            memset(buffer + seek, 0, line - seek);
            /* Put the remain chars of comment2. */
            offset += string_out(buffer + seek, descriptor[i].comment2 + offset,
                                 line - seek - 1, ' ', 0);
            printf("%s\n", buffer, offset);
        }
        
        putchar('\n');
    }

    free(buffer);
    
    return 0;
}


/**************************************************************************
 * Show the help messages
 * @param: The tool descriptor struct.
 * @param: The char numbers of every row.
 * @param: The char numbers to skip of every row.
 * @return If return 0 mean successfully, the other value means failure.
 *
 * date           author          notes
 * 2015-08-30     JiangYong       new function
 **************************************************************************/
static int descriptor_show_help(tool_descriptor * descriptor,
                                        size_t line, size_t seek)
{
    int result;
    /* Show the tool name */
    show_seek_line(descriptor->name,  line, 1, ' ');
    /* Show the tool note */
    show_seek_line(descriptor->note,  line, 1, ' ');
    /* Show the tool usage */
    show_seek_line(descriptor->usage,  line, 1, ' ');

    show_seek_line(descriptor->prompt,  line, 1, ' ');
    
    result = argument_show_help(descriptor->arguments, descriptor->total, line, seek);

    /* Show the tool example */
    show_seek_line(descriptor->example,  line, 1, ' ');

    return result;
}

/**************************************************************************
 * Initialize the full path data with the command line option.
 * param: The tool descriptor struct pointer.
 * param: The parameter descriptor struct pointer. 
 * return: If return 0 mean successfully, the other value means failure.
 *
 * date           author          notes
 * 2015-08-30     JiangYong       new function
**************************************************************************/
static int tool_show_help_info(tool_descriptor * descriptor,
                                      parameter_descriptor * table)
{
    INT state = 0;

    if (0 != descriptor_get_number(descriptor, (char *) table->option, &state))
    {
        printf(MSG_FAILED_PARAM, table->option);
        return INVALID_PARAM;
    }

    if (state)
    {
        descriptor_show_help(descriptor, descriptor->width, descriptor->seek);
        return INVALID_PARAM;
    }

    return 0;

}

/**************************************************************************
 * Initialize the full path data with the command line option.
 * param: The tool descriptor struct pointer.
 * param: The parameter descriptor struct pointer. 
 * return: If return 0 mean successfully, the other value means failure.
 *
 * date           author          notes
 * 2015-08-30     JiangYong       new function
**************************************************************************/
static int tool_get_full_path(tool_descriptor * descriptor,
                                   parameter_descriptor * table)
{
    CHAR * FullPath = NULL;

    if (0 != descriptor_get_string(descriptor, (char*)table->option, &FullPath))
    {
        printf(MSG_FAILED_PARAM, table->option);
        return INVALID_PARAM;
    }

    TOOLS_ASSERT(FullPath, return INVALID_VALUE, "Can not found parameter [%s] ...", table->option);

    memset(table->buffer, 0, table->length);
    strncpy(table->buffer, FullPath, table->length-1);
    //GetFullPathName(FullPath, table->length, (LPSTR) table->buffer, NULL);

    TOOLS_LOG("[%s] :  %p", table->option, table->buffer);

    return 0;
}

/**************************************************************************
 * Initialize the string data with the command line option.
 * param: The tool descriptor struct pointer.
 * param: The parameter descriptor struct pointer. 
 * return: If return 0 mean successfully, the other value means failure.
 *
 * date           author          notes
 * 2015-08-30     JiangYong       new function
**************************************************************************/
static int tool_get_string_data(tool_descriptor * descriptor,
                                      parameter_descriptor * table)
{
    char * string = NULL;

    if (0 != descriptor_get_string(descriptor, (char*)table->option, &string))
    {
        printf(MSG_FAILED_PARAM, table->option);
        return INVALID_PARAM;
    }

    TOOLS_ASSERT(string, return INVALID_VALUE, "Can not found parameter [%s] ...", table->option);

    if (strlen(string) > table->length)
    {
        printf(MSG_FAILED_PARAM, table->option);
        return INVALID_PARAM;
    }

    strncpy((LPSTR)table->buffer, string, table->length);

    TOOLS_LOG("[%s] :  %s", table->option, table->buffer);

    return 0;
}

/**************************************************************************
 * Initialize the bool data with the command line option.
 * param: The tool descriptor struct pointer.
 * param: The parameter descriptor struct pointer. 
 * return: If return 0 mean successfully, the other value means failure.
 *
 * date           author          notes
 * 2015-08-30     JiangYong       new function
**************************************************************************/
static int tool_get_bool_value(tool_descriptor * descriptor,
                                     parameter_descriptor * table)
{
    INT state = 0;
    BOOL * value = (BOOL *)table->buffer;

    if (0 != descriptor_get_number(descriptor, (char *) table->option, &state))
    {
        printf(MSG_FAILED_PARAM, table->option);
        return INVALID_PARAM;
    }

    TOOLS_LOG("[%s] :  %d", table->option, state);

    * value = state ? TRUE : FALSE;

    return 0;
}

/**************************************************************************
 * Initialize the integer data with the command line option.
 * param: The tool descriptor struct pointer.
 * param: The parameter descriptor struct pointer. 
 * return: If return 0 mean successfully, the other value means failure.
 *
 * date           author          notes
 * 2015-08-30     JiangYong       new function
**************************************************************************/
static int tool_get_integer_value(tool_descriptor * descriptor,
                                         parameter_descriptor * table)
{
    INT integer = 0;
    BOOL * value = (BOOL *)table->buffer;

    if (0 != descriptor_get_number(descriptor, (char *) table->option, &integer))
    {
        printf(MSG_FAILED_PARAM, table->option);
        return INVALID_PARAM;
    }

    TOOLS_LOG("[%s] :  %d", table->option, integer);

    * value = integer;

    return 0;
}

typedef int (*fnvalue)(tool_descriptor * descriptor, parameter_descriptor * table);

static const fnvalue g_fnvalue[] = {
    NULL,
    tool_show_help_info,
    tool_get_bool_value,
    tool_get_full_path,
    tool_get_integer_value,
    tool_get_string_data,
};

/**************************************************************************
 * Initialize the object with the command line option.
 * param: The tool descriptor struct pointer.
 * return: If return 0 mean successfully, the other value means failure.
 *
 * date           author          notes
 * 2015-08-30     JiangYong       new function
 * 2015-10-10     JiangYong       add fnvalue methods table
**************************************************************************/

static int tool_init_object(tool_descriptor * descriptor)
{
    int result = 0;
    unsigned int i = 0;
    parameter_descriptor * table = descriptor->arguments;

    for (i = 0; i < descriptor->total && 0 == result; i++)
    {
        fnvalue getvalue;
        unsigned int dtype = descriptor->arguments[i].dtype;

        if (dtype >= DT_MAX)
        {
            TOOLS_LOG("[%s] :  invalid table config, type is %d.", table->option, dtype);
            printf(MSG_FATAL_ERROR);
            return INTERNAL_ERROR;
        }

        getvalue = g_fnvalue[dtype];

        if (NULL != getvalue)
        {
            result = getvalue(descriptor, &table[i]);
        }
    }

    return result;
}



typedef int (*fncheck)(int argc, char * argv[], int who, parameter_descriptor * descriptor);

static const fncheck g_checkfn[] = {
    NULL,
    argument_set_status,    /* DT_HELP */
    argument_set_status,    /* DT_FLAG */
    argument_set_string,    /* DT_FILE */
    argument_set_value,     /* DT_NUMBER */
    argument_set_string,    /* DT_STRING */
};
/**
 * Scan the parameter of argument value array.
 * @param The argument count from main() function.
 * @param The argument value array from main() function.
 * @param The parameter descriptor struct array.
 * @param The number of descriptor array.
 * @return If return 0 mean successfully, the other value means failure.
 *
 * date           author          notes
 * 2015-08-30     JiangYong       new function
 */
int tool_scan_argument(int argc,char * argv[], tool_descriptor * descriptor)
{
    int i, j, result;

    TOOLS_ASSERT(argc > 0, return -1, "BUG: argc is invalid.");
    TOOLS_ASSERT(argv, return -1, "BUG: argv is NULL.");
    TOOLS_ASSERT(descriptor, return -1, "BUG: descriptor is NULL.");
    TOOLS_ASSERT(descriptor->name, return -1, "BUG: name is NULL.");
    TOOLS_ASSERT(descriptor->note, return -1, "BUG: note is NULL.");
    TOOLS_ASSERT(descriptor->usage, return -1, "BUG: usage is NULL.");
    TOOLS_ASSERT(descriptor->prompt, return -1, "BUG: prompt is NULL.");
    TOOLS_ASSERT(descriptor->example, return -1, "BUG: example is NULL.");
    TOOLS_ASSERT(descriptor->arguments, return -1, "BUG: arguments is NULL.");
    TOOLS_ASSERT(descriptor->total > 1, return -1, 
                "BUG: the count(%d) of array is invalid.", descriptor->total);

    //TOOLS_LOG("%p", descriptor->arguments[1].buffer);
    
    descriptor->path = argv[0];

    for (i = 1; i < argc; i++)
    {
        for (j = 0; j < descriptor->total; j++)
        {
            fncheck check;
            unsigned int dtype = descriptor->arguments[j].dtype;
            
            TOOLS_ASSERT(dtype < DT_MAX, return -1, "BUG: dtype(%d) is invalid.", dtype);

            check = g_checkfn[dtype];

            if (NULL == check) continue;

            result = check(argc, argv, i, &descriptor->arguments[j]);

            //TOOLS_LOG("%p", descriptor->arguments[1].buffer);

            if (INVALID_PARAM == result) continue;

            if (result >= 0)
            {
                i += result;
            }

            break;
        }

        if (result < 0)
        {
            if (INVALID_PARAM == result)
            {
                printf(MSG_INVALID_PARAM, argv[i]);
            }
            
            return result;
        }
    }

    return tool_init_object(descriptor);
}

