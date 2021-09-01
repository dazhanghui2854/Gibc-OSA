

#include <osa.h>


static Char xtod(Char c) {
    if (c>='0' && c<='9') return c-'0';
    if (c>='A' && c<='F') return (c-'A')+10;
    if (c>='a' && c<='f') return (c-'a')+10;
    return 0;        /* not Hex digit */
}
  
static Int32 HextoDec(Char *hex, Int32 l)
{
    if (*hex==0) 
        return(l);

    return HextoDec(hex+1, l*16+xtod(*hex)); /* hex+1? */
}
  
Int32 OSA_xstrtoi(Char *hex)      /* hex string to integer */
{
    return HextoDec(hex,0);
}

#ifdef __KERNEL__
EXPORT_SYMBOL(OSA_xstrtoi);
#endif


