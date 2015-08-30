#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libtools.h"
#include "language.h"



#if 0
static int get_filename(int argc,char * argv[], char * text, char * * dest)
{
    int i;
    for(i = 1; i <argc - 1; i++)
    {
        if(strcmp(argv[i], text))
        {
            continue;
        }

        if(argv[i + 1] == NULL)
        {
            *dest = argv[i];
            return NO_FILE;
        }

        *dest = argv[i + 1];

        if(*argv[i + 1] == '\0')
        {
            *dest = argv[i];
            return NO_FILE;
        }

        if(strchr(argv[i + 1], '*') || strchr(argv[i + 1], '?'))
        {
            *dest = argv[i + 1];
            return INVALID_FILE;
        }

        if(*(argv[i + 1])=='-')
        {
            *dest = argv[i + 1];
            return INVALID_PARAM;
        }
        break;
    }

    return 0;
}

#endif

/**
 * Check the argument and set help flag.
 * param: The argument count from main() function.
 * param: The argument value array from main() function.
 * param: The index for argument value array.
 * param: The parameter descriptor struct pointer.
 * @return If return 0 mean successfully, the other value means failure.
 *
 * \par
 *     If this argument is '-h', '-H', '--h', '--H', '/?', '-?', '--help',
 * we will be set the value member of descriptor to 1. we can check the help
 * value to determine show the help messages.
 *
 * date           author          notes
 * 2015-08-30     JiangYong       new function
 */
int argument_set_help(int argc, char * argv[], int who, parameter_descriptor * descriptor)
{
    int i;
    char * labels[] = {"-h", "-H", "--h", "--H", "/?", "-?", "--help"};

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

/**
 * Check the argument and set the string menber of descriptor.
 * param: The argument count from main() function.
 * param: The argument value array from main() function.
 * param: The index for argument value array.
 * param: The parameter descriptor struct pointer.
 * @return If return 0 mean successfully, the other value means failure.
 *
 * \par
 *     If this argument argv[who] equal to the label menber of this descriptor,
 * we will be set the string member of descriptor to argv[who + 1].
 *
 * date           author          notes
 * 2015-08-30     JiangYong       new function
 */
static int argument_set_string(int argc, char * argv[], int who, parameter_descriptor * descriptor)
{
    if (0 != strcmp(descriptor->label, argv[who]))
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

/**
 * Check the argument and set the value menber of descriptor.
 * param: The argument count from main() function.
 * param: The argument value array from main() function.
 * param: The index for argument value array.
 * param: The parameter descriptor struct pointer.
 * @return If return 0 mean successfully, the other value means failure.
 *
 * \par
 *     If this argument argv[who] equal to the label menber of this descriptor,
 * we will be set the value member of descriptor to argv[who + 1].
 *
 * date           author          notes
 * 2015-08-30     JiangYong       new function
 */
int argument_set_value(int argc, char * argv[], int who, parameter_descriptor * descriptor)
{
    char * next = NULL;

    if (0 != strcmp(descriptor->label, argv[who]))
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

/**
 * Check the argument and set the value menber of descriptor.
 * param: The argument count from main() function.
 * param: The argument value array from main() function.
 * param: The index for argument value array.
 * param: The parameter descriptor struct pointer.
 * @return If return 0 mean successfully, the other value means failure.
 *
 * \par
 *     If this argument argv[who] equal to the label menber of this descriptor,
 * we will be set the value member of descriptor to 1.
 *
 * date           author          notes
 * 2015-08-30     JiangYong       new function
 */
int argument_set_status(int argc, char * argv[], int who, parameter_descriptor * descriptor)
{
    if (0 != strcmp(descriptor->label, argv[who]))
    {
        return INVALID_PARAM;
    }

    descriptor->value = 1;

    return 0;
}

/**
 * Search the argument and return the string member of descriptor
 * param: The parameter descriptor struct array.
 * param: The number of descriptor struct array;
 * param: The label want to serach.
 * param: The string will be return.
 * @return If return 0 mean successfully, the other value means failure.
 *
 * date           author          notes
 * 2015-08-30     JiangYong       new function
 */
int argument_get_string(parameter_descriptor descriptor[], int count, char * label, char * * string)
{
    int i;
    
    tools_assert(count > 0, return -1, "BUG: count(%d) is invalid.", count);
    tools_assert(NULL != string, return -1, "BUG: string is NULL.");
    
    for (i = 0; i < count; i ++)
    {
        if (descriptor[i].label == label)
        {
            if (NULL != descriptor[i].string)
            {
                *string = descriptor[i].string;
                return 0;
            }
            
            return INVALID_VALUE;
        }
    }
    
    return NOT_FOUND;
}

/**
 * Search the argument and return the value member of descriptor.
 * @param: The tool descriptor struct.
 * @param: The label want to serach.
 * @param: The value will be return.
 * @return If return 0 mean successfully, the other value means failure.
 *
 * date           author          notes
 * 2015-08-30     JiangYong       new function
 */
int descriptor_get_string(tool_descriptor * descriptor, char * label, char * * string)
{
    return argument_get_string(descriptor->arguments, descriptor->total, label, string);
}

/**************************************************************************
 * Search the argument and return the value member of descriptor.
 * param: The parameter descriptor struct array.
 * param: The number of descriptor struct array;
 * param: The label want to serach.
 * param: The value will be return.
 * @return If return 0 mean successfully, the other value means failure.
 *
 * date           author          notes
 * 2015-08-30     JiangYong       new function
 **************************************************************************/
static int argument_get_value(parameter_descriptor descriptor[], int count, char * label, int * value)
{
    int i;

    tools_assert(label, return INVALID_PARAM, "BUG: label is NULL.");
    tools_assert(value, return INVALID_PARAM, "BUG: value is NULL.");
    
    for (i = 0; i < count; i ++)
    {
        if (descriptor[i].label == label)
        {
            *value = descriptor[i].value;
            return 0;
        }
    }

    return NOT_FOUND;
}

/**
 * Search the argument and return the value member of descriptor.
 * @param: The tool descriptor struct.
 * @param: The label want to serach.
 * @param: The value will be return.
 * @return If return 0 mean successfully, the other value means failure.
 *
 * date           author          notes
 * 2015-08-30     JiangYong       new function
 */
int descriptor_get_number(tool_descriptor * descriptor, char * label, int * value)
{
    return argument_get_value(descriptor->arguments, descriptor->total, label, value);
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
                       int count, size_t line, size_t skip)
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
        /* Put the label member of descriptor struct */
        scale += string_out(buffer + scale, descriptor[i].label, line, ' ', 1);
        /* Put the comment1 member of descriptor struct */
        scale += string_out(buffer + scale, descriptor[i].comment1, line, ' ', 1);

        length = strlen(descriptor[i].comment2);

        if (scale >= skip)
        {
            /* If scale out of skip, put the buffer to screen and the */
            /* comment2 will be put to next row.*/
            printf("%s\n", buffer);
        }
        else
        {
            /* If scale not out of skip, the comment2 will */
            /* be put to the end of current row.*/
            memset(buffer + scale, ' ', skip - scale);
            offset = string_out(buffer + skip, descriptor[i].comment2,
                                line - skip - 1, ' ', 0);
            printf("%s\n", buffer);
        }

        /* Set the skip chars to space.*/
        memset(buffer, ' ', skip);

        while(offset < length)
        {
            memset(buffer + skip, 0, line - skip);
            /* Put the remain chars of comment2. */
            offset += string_out(buffer + skip, descriptor[i].comment2 + offset,
                                 line - skip - 1, ' ', 0);
            printf("%s\n", buffer, offset);
        }
        
        putchar('\n');
    }
    
    return 0;
}

/**
 * Show the help messages
 * @param: The tool descriptor struct.
 * @param: The char numbers of every row.
 * @param: The char numbers to skip of every row.
 * @return If return 0 mean successfully, the other value means failure.
 *
 * date           author          notes
 * 2015-08-30     JiangYong       new function
 */
int descriptor_show_help(tool_descriptor * descriptor, size_t line, size_t skip)
{
    int result;
    /* Show the tool name */
    printf("%s\n\n", descriptor->name);
    /* Show the tool usage */
    printf("%s\n\n", descriptor->usage);
    
    /* Show the tool notes */
    printf("%s\n\n", NOTES_HELP);
    
    result = argument_show_help(descriptor->arguments, descriptor->total, line, skip);

    /* Show the tool example */
    printf("%s\n\n", descriptor->example);
    
    return result;
}


static const fncheck g_checkfn[] = {
    NULL,
    argument_set_help,      /* DT_HELP */
    argument_set_status,    /* DT_FLAG */
    NULL,                   /* DT_FILE */
    argument_set_value,     /* DT_VALUE */
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
int descriptor_scan_argument(int argc,char * argv[], tool_descriptor * descriptor)
{
    int i, j, result;

    tools_assert(argc > 0, return -1, "BUG: argc is invalid.");
    tools_assert(argv, return -1, "BUG: argv is NULL.");
    tools_assert(descriptor, return -1, "BUG: descriptor is NULL.");
    tools_assert(descriptor->name, return -1, "BUG: name is NULL.");
    tools_assert(descriptor->usage, return -1, "BUG: usage is NULL.");
    tools_assert(descriptor->example, return -1, "BUG: example is NULL.");
    tools_assert(descriptor->arguments, return -1, "BUG: arguments is NULL.");
    tools_assert(descriptor->total > 1, return -1, 
                "BUG: the count(%d) of array is invalid.", descriptor->total);
    
    for (i = 1; i < argc; i++)
    {
        for (j = 0; j < descriptor->total; j++)
        {
            fncheck check;
            unsigned int dtype = descriptor->arguments[j].dtype;
            
            tools_assert(dtype < DT_MAX, return -1, "BUG: dtype(%d) is invalid.", dtype);

            check = g_checkfn[dtype];
            if (NULL == check) continue;


            result = check(argc, argv, i, &descriptor->arguments[j]);

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

    return 0;
}

