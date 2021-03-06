

#ifndef _OSA_FLG_H_
#define _OSA_FLG_H_

#ifdef __cplusplus
extern "C" {
#endif

#if 0
typedef struct {

    Uint32 pattern;
    pthread_mutex_t lock;
    pthread_cond_t  cond;

} OSA_FlgHndl;
#endif

typedef Handle OSA_FlgHndl;

/* flags */
#define OSA_FLG_MODE_AND   0x1
#define OSA_FLG_MODE_OR    0x2
#define OSA_FLG_MODE_CLR   0x4

#define OSA_FLG_MODE_AND_CLR   (OSA_FLG_MODE_AND|OSA_FLG_MODE_CLR)
#define OSA_FLG_MODE_OR_CLR    (OSA_FLG_MODE_OR |OSA_FLG_MODE_CLR)

Int32  OSA_flgCreate(OSA_FlgHndl *pHndl, Uint32 initPattern);
Int32  OSA_flgWait(OSA_FlgHndl hndl, Uint32 pattern,
                    Uint32 mode, Uint32 timeout);
Int32  OSA_flgSet(OSA_FlgHndl hndl, Uint32 pattern);
Int32  OSA_flgClear(OSA_FlgHndl hndl, Uint32 pattern);
Bool OSA_flgIsSet(OSA_FlgHndl hndl, Uint32 pattern);
Int32  OSA_flgDelete(OSA_FlgHndl hndl);

#ifdef __cplusplus
}
#endif

#endif /* _OSA_FLG_H_ */



