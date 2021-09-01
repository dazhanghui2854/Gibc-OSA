

#ifndef _OSA_UWAIT_H_
#define _OSA_UWAIT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*仅linux用户态支持以下接口*/
#if !defined(__KERNEL__) && !defined(___DSPBIOS___)
Int32 OSA_select(Int32 fd, Int32 usec);
#endif

#ifdef __cplusplus
}
#endif

#endif


