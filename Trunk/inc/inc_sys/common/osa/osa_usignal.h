

#ifndef _OSA_USIGNAL_H_
#define _OSA_USIGNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

/*仅linux用户态支持以下接口*/
#if !defined(__KERNEL__) && !defined(___DSPBIOS___)

Int32  OSA_attachSignalHandler(Int32 sigId, void (*handler)(Int32) );
#endif

#ifdef __cplusplus
}
#endif

#endif


