#include "StdAfx.h"
#include "Translator.h"

static LanguageCode _BingLanguageCodeInfo[] =
{
   { "Arabic",       "ar" },
   { "Bosnian",      "bs-Latn"},
   { "Bulgarian",    "bg"},

   { "Catalan",      "ca"},
   { "Chinese Simplified", "zh-CHS"},
   { "Chinese Traditional", "zh-CHT"},
   { "Croatian",     "hr"},
   { "Czech",        "cs"},

   { "Danish",       "da"},
   { "Dutch",        "nl"},
   { "English",      "en"},
   { "Estonian",     "et"},

   { "Finnish",      "fi"},
   { "French",       "fr"},

   { "German",       "el"},
   { "Greek",        "fi"},

   { "Haitian Creole", "ht"},
   { "Hebrew",       "he"},
   { "Hindi",        "hi"},
   { "Hmong",        "mww"},
   { "Hungarian",    "hu"},

   { "Indonesian",   "id"},
   { "Italian",      "it"},

   { "Japanese",     "ja"},
   { "Korean",       "ko"},

   { "Latvian",      "lv"},
   { "Lithuanian",   "lt"},

   { "Malay",        "ms"},
   { "Maltese",      "mt"},
   { "Norwegian",    "no"},
   { "Persian",      "fa"},
   { "Polish",       "pl"},
   { "Portuguese",   "pt"},

   { "Romanian",     "ro"},
   { "Russian",      "ru"},
   { "Serbian",      "sr-Cyrl"},
   { "Slovak",       "sk"},
   { "Slovenian",    "sl"},
   { "Spanish",      "es"},
   { "Swedish",      "sv"},

   { "Thai",         "th"},
   { "Turkish",      "tr"},
   { "Ukrainian",    "uk"},
   { "Urdu",         "ur"},

   { "Vietnamese",   "vi"},
   { "Welsh",        "cy"}
};

CTranslator::CTranslator(void)
{
   m_hInternet = NULL;
   m_sToLangCode = _T("hi");

   // Add your ID and Key
   MY_ID = "";
   MY_KEY = "";
   
   m_sAccessToken = "";
   m_iExpirationTime = 0;
   m_iTokenTime = 0;
}

CTranslator::~CTranslator(void)
{
   if(m_hInternet)
      InternetCloseHandle(m_hInternet);
}

CString CTranslator::ToUnicodeText(const char* buffer)
{
   CString trans_str;
   trans_str.Empty();

   int nChars = MultiByteToWideChar(CP_UTF8, 0, buffer, -1, NULL, 0);
   // allocate it
   if (nChars == 0) return trans_str;

   WCHAR *pwcsName;
   pwcsName = new WCHAR[nChars+1];

   MultiByteToWideChar(CP_UTF8, 0, buffer, -1, (LPWSTR)pwcsName, nChars);

   trans_str = pwcsName;

   delete [] pwcsName;

   return trans_str;
}

HINTERNET CTranslator::OpenInternet()
{
   LPCTSTR lpszAgent = L"WinInetGet/0.1";   // User agent  - Application name
   HINTERNET hInternet = InternetOpen(lpszAgent,
      INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

   if(hInternet == NULL)
   {
      m_sErrorMsg = _T("Internet connection failed\n");
      return NULL;
   }
   return hInternet;
}

int CTranslator::GetTranslation(CString& org_str, CString& trans_str, CString& sErrorMsg)
{
   if(m_hInternet == NULL)
   {
      m_hInternet = OpenInternet();
      if(m_hInternet == NULL)
      {
         sErrorMsg = m_sErrorMsg;
         trans_str.Empty();
         return 0;
      }
   }

   m_iExpirationTime = m_iTokenTime - ( GetTickCount64() / 1000);

   if(m_iExpirationTime <= 0)
   {
      m_sAccessToken.Trim();

      GetAccessToken(m_hInternet,MY_ID, MY_KEY, m_sAccessToken, &m_iExpirationTime);

      m_iTokenTime = m_iExpirationTime + GetTickCount64() / 1000;

      m_sAccessToken.Trim();
      if(m_sAccessToken.IsEmpty())
      {
         sErrorMsg = m_sErrorMsg;
         return 0;
      }
   }

   if(GetTranslatedString(m_hInternet, m_sAccessToken, org_str, _T("en"), m_sToLangCode, trans_str))
   {
      USES_CONVERSION;
      char * temp = T2A(trans_str);
      trans_str = ToUnicodeText(temp);
      return 1;
   }
   else
      sErrorMsg = _T("Error Translating some sentences");

   return 0;
}

void CTranslator::GetAccessToken(__in HINTERNET hInternet,__in const CString& my_id,__in const CString& my_secret, __out CString& aAccessToken, __out int* iExpirationTime)
{
   LPCTSTR        lpszServerName =  L"datamarket.accesscontrol.windows.net"; 
   INTERNET_PORT  nServerPort = INTERNET_DEFAULT_HTTPS_PORT;
   LPCTSTR        lpszUserName = NULL;
   LPCTSTR        lpszPassword = NULL;
   DWORD          dwService = INTERNET_SERVICE_HTTP;
   DWORD          dwConnectFlags = INTERNET_FLAG_RESYNCHRONIZE;
   DWORD          dwConnectContext = 0;

   HINTERNET hConnect = InternetConnect(
      hInternet,
      lpszServerName,
      nServerPort,
      lpszUserName, 
      lpszPassword,
      dwService,
      dwConnectFlags, 
      dwConnectContext
      );

   if(hConnect == NULL)
   {
      m_sErrorMsg = _T("Couldn't connect to Internet\n");
      return;
   }

   LPCTSTR lpszVerb = L"POST";
   LPCTSTR lpszObjectName = L"/v2/OAuth2-13"; 
   LPCTSTR lpszVersion = NULL;      // Use default.
   LPCTSTR lpszReferrer = NULL;     // No referrer.
   LPCTSTR lplpszAcceptTypes[] = {L"application/json", NULL};  // A pointer to a null-terminated array of strings.

   DWORD dwOpenRequestFlags = 
      INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP |
      INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS |
      INTERNET_FLAG_KEEP_CONNECTION |
      INTERNET_FLAG_PRAGMA_NOCACHE |
      INTERNET_FLAG_NO_CACHE_WRITE |
      INTERNET_FLAG_NO_COOKIES |
      INTERNET_FLAG_NO_UI |
      INTERNET_FLAG_RELOAD|
      INTERNET_FLAG_SECURE;

   DWORD dwOpenRequestContext = 1;

   HINTERNET hRequest = HttpOpenRequest(hConnect, lpszVerb, lpszObjectName, lpszVersion, lpszReferrer,
      lplpszAcceptTypes, dwOpenRequestFlags, dwOpenRequestContext );

   if(hRequest == NULL)
   {
      m_sErrorMsg = _T("Couldn't connect to Internet\n");
      InternetCloseHandle(hConnect);
      return;
   }

   //4. HttpSendRequest
   CString   lpszHeaders;
   DWORD     dwHeadersLength = 0;
   LPVOID    lpOptional = NULL;   // it is generally used for POST and PUT operations.
   DWORD     dwOptionalLength = 0;

   CString encoded_ID, encoded_Secret ;

   EncodeStringToURLFormat(my_id, encoded_ID);
   EncodeStringToURLFormat(my_secret, encoded_Secret);

   // Create Request body
   CString temp ;
   temp.Format(
      _T("grant_type=client_credentials")
      _T("&client_id=%s")
      _T("&client_secret=%s")
      _T("&scope=http://api.microsofttranslator.com"),
      encoded_ID, encoded_Secret
      );
   USES_CONVERSION;
   char szFormData[4096];
   strcpy(szFormData,T2A(temp));
   lpOptional = szFormData;
   dwOptionalLength = strlen(szFormData);

   temp.Format(
      _T("Content-Type: application/x-www-form-urlencoded\r\n")
      _T("Host: datamarket.accesscontrol.windows.net\r\n")
      _T("Content-Length: %d\r\n")
      _T("Expect: 100-continue\r\n")
      _T("Connection: keep-alive\r\n")
      _T("\r\n\r\n"),
      strlen(szFormData)
      );

   lpszHeaders = temp;
   dwHeadersLength = lpszHeaders.GetLength();

   BOOL bResult = HttpSendRequest(hRequest, lpszHeaders, dwHeadersLength, szFormData, dwOptionalLength );

   if(bResult == FALSE)
   {
//       printf("HttpSendRequest Failed or Syntax error\n");
//       printf("Error = %d\n", GetLastError());
      m_sErrorMsg = _T("Couldn't connect to Internet\n");
      InternetCloseHandle(hRequest);
      InternetCloseHandle(hConnect);
      return;
   }

   // Get Xml formated response (typically ~600 chars in length)
   char szResponse[4096];
   DWORD dwBytesRead = 0;
   if(InternetReadFile(hRequest, szResponse,strlen(szResponse), &dwBytesRead) && (dwBytesRead > 0))
   {
      szResponse[dwBytesRead] = 0;

      // Parse the data
      // Access token
      const char ACTSTART[] = {"\"access_token\":\""};
      const char ENDTAG[]   = {"\","};
      if(LPSTR pszACToken = strstr(szResponse, ACTSTART))
      {
         pszACToken += strlen(ACTSTART);
         if(LPSTR pszEnd = strstr(pszACToken + 32, ENDTAG))
         {
            *pszEnd = 0;

            aAccessToken = pszACToken;

            // Token expiration seconds
            const char EXINSTART[] = {"\"expires_in\":\""};
            if(LPSTR pszExpiresStart = strstr(pszEnd + strlen(ENDTAG), EXINSTART))
            {
               pszExpiresStart += strlen(EXINSTART);
               if(LPSTR pszEnd = strchr(pszExpiresStart, '"'))
               {
                  *pszEnd = 0;
                  *iExpirationTime = atoi(pszExpiresStart);
               }
            }
         }
      }
   }

   InternetCloseHandle(hRequest);
   InternetCloseHandle(hConnect);
   return;
}

int CTranslator::GetTranslatedString(__in HINTERNET hInternet,__in const CString& aAccessToken, __in const CString& orig_str,
   __in const CString& fromLang, __in const CString& ToLang, __out CString& translated_str)
{
   CString    lpszUrl;
   CString    lpszHeaders;
   DWORD    dwHeadersLength = 0;
   DWORD_PTR dwContext = 0;

   translated_str.Empty();

   CString encoded_org_str;

   EncodeStringToURLFormat(orig_str, encoded_org_str);

   lpszUrl.Format(_T("http://api.microsofttranslator.com/v2/Http.svc/Translate?text=%s&from=%s&to=%s"), encoded_org_str, fromLang, ToLang);

   lpszHeaders.Format(_T("Authorization: Bearer %s\r\n"), aAccessToken);
   lpszHeaders += _T("Host: api.microsofttranslator.com\r\n");
   lpszHeaders += _T("Connection: keep-alive\r\n");
   lpszHeaders += _T("\r\n\r\n");

   //dwHeadersLength = wcslen(lpszHeaders);
   dwHeadersLength = lpszHeaders.GetLength();

   const UINT dwFlags =
      (
      INTERNET_FLAG_KEEP_CONNECTION |
      INTERNET_FLAG_NO_CACHE_WRITE |
      INTERNET_FLAG_NO_COOKIES |
      INTERNET_FLAG_NO_UI |
      INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS
      );

   HINTERNET hOpenUrl = InternetOpenUrl(hInternet, lpszUrl, lpszHeaders, dwHeadersLength, dwFlags, dwContext);

   if(hOpenUrl == NULL)
   {
      printf("InternetOpenUrl Failed or Syntax error\n");
      return 0;
   }

   // Read response
   DWORD dwBytesAvailable;
   char *pMessageBody = NULL;
   int   nMessageLen = 0;
   BOOL  err = FALSE;
   while(InternetQueryDataAvailable(hOpenUrl, &dwBytesAvailable, 0, 0))
   {
      char * temp = (char *)malloc(dwBytesAvailable+1);

      DWORD dwBytesRead;

      BOOL bResult = InternetReadFile(hOpenUrl, temp,
         strlen(temp), &dwBytesRead);

      if (!bResult)
      {
         printf("InternetReadFile failed, error = %d (0x%x)\n",
            GetLastError(), GetLastError());
         free(temp);
         err = TRUE;
         break;
      }

      temp[dwBytesRead] = '\0';

      if(dwBytesRead == 0)
      {
         free(temp);
         break;
      }
      if(nMessageLen == 0)
      {
         pMessageBody  = (char *) malloc (strlen(temp) + 1);
         strcpy(pMessageBody,temp);
      }
      else
      {
         pMessageBody  = (char *) realloc (pMessageBody, nMessageLen + strlen(temp) + 1);
         strcat(pMessageBody,temp);
      }
      nMessageLen = strlen(pMessageBody);

      free(temp);
   }

   BOOL isTranslated = FALSE;
   if(err == FALSE)
   {
      // Parse out XML formated string
      char* pszString = strstr(pMessageBody, "<string xmlns=\"http://schemas.microsoft.com/2003/10/Serialization/\">");
      if(pszString)
      {
         pszString += strlen("<string xmlns=\"http://schemas.microsoft.com/2003/10/Serialization/\">");
         if(LPSTR pszEnd = strstr(pszString, "</string>"))
         {
            *pszEnd = 0;
            if(strlen(pszString))
            {
               translated_str = pszString;
               isTranslated = TRUE;
            }
         }
      }
   }

   if(isTranslated == FALSE)
   {
      char szHeader[1024]; szHeader[0] = 0;
      DWORD dwHeaderSize  = sizeof(szHeader);
      if(HttpQueryInfo(hOpenUrl, HTTP_QUERY_RAW_HEADERS_CRLF, szHeader,&dwHeaderSize, NULL))
         szHeader[dwHeaderSize] = 0;

      printf( "Translate response parse failure!\nHeader:\n%s\"\"\nResponse data:\n%s\"",szHeader,pMessageBody);
   }

   if(nMessageLen)
      free(pMessageBody);

   InternetCloseHandle(hOpenUrl);

   if(isTranslated == FALSE)
      return 0;
   else
      return 1;
}


int CTranslator::EncodeStringToURLFormat(const CString& org_str, CString& encoded_str)
{
   int iInputSize = org_str.GetLength();
   encoded_str.Empty();

   // RFC 3986 section 2.3, reserved, unreserved, and space character routing
   __declspec(align(16)) static const BYTE aCharRoute[256] =
   {
      3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
      1,2,2,2,2,2,2,2,2,2,2,2,2,0,0,2,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,
      2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,
      2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,0,2,
      2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
      2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
      2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
      2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
   };
   // To spec A-F can be upper or lower case but the later seems more standard/popular
   static const BYTE aN2Hex[] = {"0123456789abcdef"};

   for (int i = 0; i < iInputSize; i++)
   {
      BYTE c = org_str.GetAt(i);
      switch(aCharRoute[c])
      {
         // Pass along unreserved characters
         // Characters A-Z, a-z, 0-9
      case 0:
         encoded_str.AppendChar(c);
         break;

         // Encode space character
      case 1:
         encoded_str.AppendChar('+');
         break;

         // Percent encode reserved characters
         // unsafe characters
      case 2:
         {
            encoded_str.AppendChar('%');
            encoded_str.AppendChar(aN2Hex[c >> 4]);
            encoded_str.AppendChar(aN2Hex[c & 0xF]);
         }
         break;

         // Bail on terminator
      case 3:
         iInputSize = 0;
         break;
      };
   }

   if(encoded_str.IsEmpty())
      return 0;
   return 1;
}

void  CTranslator::GetAllLanguages(CStringArray& sLanguages)
{
   sLanguages.RemoveAll();
   int count = sizeof(_BingLanguageCodeInfo) / sizeof(LanguageCode);
   for (int i = 0 ; i < count; i++)
   {
      CString stemp(_BingLanguageCodeInfo[i].sLanguage);
      sLanguages.Add(stemp);
   }
}

int CTranslator::GetLanguageCodeGivenLanguage(const CString& sLanguage, CString& sLangCode)
{
   int count = sizeof(_BingLanguageCodeInfo) / sizeof(LanguageCode);

   for(int i = 0; i < count; i++)
   {
      if(_BingLanguageCodeInfo[i].sLanguage == sLanguage)
      {
         sLangCode = _BingLanguageCodeInfo[i].sLanguageCode;
         return 1;
      }
   }
   return 0;
}

void  CTranslator::SetToLanguage(CString& sToLang)
{
   m_sToLangCode = _T("kn");
   GetLanguageCodeGivenLanguage(sToLang, m_sToLangCode);
}
