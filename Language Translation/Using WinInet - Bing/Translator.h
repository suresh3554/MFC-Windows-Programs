#pragma once

#include<WinInet.h> 

struct LanguageCode
{
   char sLanguage[256];
   char sLanguageCode[16];
};


class CTranslator
{
   HINTERNET   m_hInternet;
   CString     m_sToLangCode;
   CString     m_sErrorMsg;

   CString     MY_ID, MY_KEY;
   CString     m_sAccessToken;
   int         m_iTokenTime;
   int         m_iExpirationTime;


   HINTERNET   OpenInternet();
   void        GetAccessToken(__in HINTERNET hInternet,__in const CString& my_id,__in const CString& my_secret, __out CString& aAccessToken, __out int* iExpirationTime);

   CString     ToUnicodeText(const char* buffer);

   int         GetTranslatedString(__in HINTERNET hInternet,__in const CString& aAccessToken, __in const CString& orig_str,
               __in const CString& fromLang, __in const CString& ToLang, __out CString& translated_str);

   int         EncodeStringToURLFormat(const CString& org_str, CString& encoded_str);

   int         GetLanguageCodeGivenLanguage(const CString& sLanguage, CString& sLangCode);

public:

   CTranslator(void);
   ~CTranslator(void);

   void        GetAllLanguages(CStringArray& sLanguages);

   int         GetTranslation(CString& org_str, CString& trans_str, CString& sErrorMsg);

   void        SetToLanguage(CString& sToLang);
};

