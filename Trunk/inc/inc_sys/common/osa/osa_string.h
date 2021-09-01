#ifndef _OSA_STRING_H_
#define _OSA_STRING_H_

#ifdef __KERNEL__    /* Linux kernel */

#define OSA_strtoul     simple_strtoul
#define OSA_strtol      simple_strtol
#define OSA_strtoull    simple_strtoull
#define OSA_strtoll     simple_strtoll

#else   /* Linux user And TI SYSBIOS */

#define OSA_strtoul     strtoul
#define OSA_strtol      strtol
#define OSA_strtoull    strtoull
#define OSA_strtoll     strtoll

#endif

#define OSA_sscanf      sscanf
#define OSA_strstr      strstr



#define OSA_strlen(pName)   \
                strlen(pName)

#define OSA_strcpy(sDest, sSrc)   \
                strcpy(sDest, sSrc)
#define OSA_strcmp(sSrcA, sSrcB)  \
                strcmp(sSrcA, sSrcB)
#define OSA_strcat(sDest, sSrc)   \
                strcat(sDest, sSrc)

#define OSA_strncpy(sDest, sSrc, size)   \
                strncpy(sDest, sSrc, size)
#define OSA_strncmp(sSrcA, sSrcB, size)  \
                strncmp(sSrcA, sSrcB, size)
#define OSA_strncat(sDest, sSrc, size)          \
                strncat(sDest, sSrc, size)


#ifndef _WIN32
#define OSA_sprintf(sDest, fmt, args ...) \
                sprintf(sDest, fmt, ## args)
#define OSA_snprintf(sDest, size, fmt, args ...) \
                snprintf(sDest, size, fmt, ## args)

#else
#define OSA_sprintf(sDest, fmt, ...) \
	sprintf(sDest, fmt, ##__VA_ARGS__)
#define OSA_snprintf(sDest, size, fmt, ...) \
	snprintf(sDest, size, fmt, ##__VA_ARGS__)

#endif

Int32 OSA_xstrtoi(char *hex);


#define OSA_strtok(str, delim)  \
                strtok(str, delim)


#endif

