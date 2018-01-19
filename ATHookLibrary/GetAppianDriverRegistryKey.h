
#ifndef _GETAPPIANDRIVERREGISTRYKEY_H_INCLUDED_

#define _GETAPPIANDRIVERREGISTRYKEY_H_INCLUDED_ 

#ifdef __cplusplus

extern "C" {

#endif


    DWORD GetAppianDriverRegistryKey(IN TCHAR * ptszAppianDriverKey,
        IN size_t t_BufferSize);

#ifdef __cplusplus

}
#endif


#endif  // _GETAPPIANDRIVERREGISTRYKEY_H_INCLUDED_
