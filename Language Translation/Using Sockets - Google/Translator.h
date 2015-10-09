#pragma once

#include<WinInet.h> 

struct LanguageCode
{
   char sLanguage[256];
   char sLanguageCode[16];
};


class CTranslator
{
   int         m_clientSocket;
   CString     m_sToLangCode;
   CString     m_sErrorMsg;

   int         LoadSocketDllsandLibs();

   CString     ToUnicodeText(const char* buffer);

   int         GetTranslatedString(const CString& orig_str, __in const CString& fromLang, __in const CString& ToLang, __out CString& translated_str);
   
   int         EncodeStringToURLFormat(const CString& org_str, CString& encoded_str);

   int         GetLanguageCodeGivenLanguage(const CString& sLanguage, CString& sLangCode);

public:

   CTranslator(void);
   ~CTranslator(void);


   int         CreateAndConnectSocket();

   void        GetAllLanguages(CStringArray& sLanguages);

   int         GetTranslation(CString& org_str, CString& trans_str, CString& sErrorMsg);

   void        SetToLanguage(CString& sToLang);

   CString     GetErrorMessage(){return m_sErrorMsg;}
};

