#include "StdAfx.h"
#include "Translator.h"

static LanguageCode _GoogleLanguageCodeInfo[] =
{
   { "Afrikaans",    "af" },
   { "Albanian",     "sq" },
   { "Arabic",       "ar" },
   { "Armenian",     "hy" },
   { "Azerbaijani",  "az" },

   { "Basque",       "eu"},
   { "Belarusian",   "be"},
   { "Bengali",      "bn"},
   { "Bosnian",      "bs"},
   { "Bulgarian",    "bg"},

   { "Catalan",      "ca"},
   { "Cebuano",      "ceb"},
   { "Chichewa",     "ny"},
   { "Chinese Simplified", "zh-CN"},
   { "Chinese Traditional", "zh-TW"},
   { "Croatian",     "hr"},
   { "Czech",        "cs"},

   { "Danish",       "da"},
   { "Dutch",        "nl"},
   { "English",      "en"},
   { "Esperanto",    "eo"},
   { "Estonian",     "et"},

   { "Filipino",     "tl"},
   { "Finnish",      "fi"},
   { "French",       "fr"},

   { "Galician",     "gl"},
   { "Georgian",     "ka"},
   { "German",       "el"},
   { "Greek",        "fi"},
   { "Gujarati",     "gu"},

   { "Haitian Creole", "ht"},
   { "Hausa",        "ha"},
   { "Hebrew",       "iw"},
   { "Hindi",        "hi"},
   { "Hmong",        "hmn"},
   { "Hungarian",    "hu"},

   { "Icelandic",    "is"},
   { "Igbo",         "ig"},
   { "Indonesian",   "id"},
   { "Irish",        "ga"},
   { "Italian",      "it"},

   { "Japanese",     "ja"},
   { "Javanese",     "jw"},
   { "Kannada",      "kn"},
   { "Kazakh",       "kk"},
   { "Khmer",        "km"},
   { "Korean",       "ko"},

   { "Lao",          "lo"},
   { "Latin",        "la"},
   { "Latvian",      "lv"},
   { "Lithuanian",   "lt"},
   { "Macedonian",   "mk"},
   { "Malagasy",     "mg"},

   { "Malay",        "ms"},
   { "Malayalam",    "ml"},
   { "Maltese",      "mt"},
   { "Maori",        "mi"},
   { "Marathi",      "mr"},
   { "Mongolian",    "mn"},
   { "Myanmar (Burmese)", "my"},

   { "Nepali",       "ne"},
   { "Norwegian",    "no"},
   { "Persian",      "fa"},
   { "Polish",       "pl"},
   { "Portuguese", "pt"},

   { "Punjabi",      "ma"},
   { "Romanian",     "ro"},
   { "Russian",      "ru"},
   { "Serbian",      "sr"},
   { "Sesotho", "st"},

   { "Sinhala",      "si"},
   { "Slovak",       "sk"},
   { "Slovenian",    "sl"},
   { "Somali",       "so"},
   { "Spanish", "es"},

   { "Sudanese",     "su"},
   { "Swahili",      "sw"},
   { "Swedish",      "sv"},
   { "Tajik",        "tg"},
   { "Tamil",        "ta"},

   { "Telugu",       "te"},
   { "Thai",         "th"},
   { "Turkish",      "tr"},
   { "Ukrainian",    "uk"},
   { "Urdu",         "ur"},

   { "Uzbek",        "uz"},
   { "Vietnamese",   "vi"},
   { "Welsh",        "cy"},
   { "Yiddish",      "yi"},
   { "Yoruba",       "yo"},
   { "Zulu",         "zu"},
};

CTranslator::CTranslator(void)
{
   m_hInternet = NULL;
   m_sToLangCode = _T("kn");
}

CTranslator::~CTranslator(void)
{
   if(m_hInternet)
      InternetCloseHandle(m_hInternet);
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

   CString      sTemp, trans_text ;
   CStringArray sTexts;
   int curPos = 0, tokenCount = 0, ret = 0;

   sErrorMsg.Empty();

   sTemp= org_str.Tokenize(_T("."),curPos);
   sTemp.Trim();

   while (sTemp != _T(""))
   {
      sTexts.Add(sTemp);
      if(curPos < org_str.GetLength())
         tokenCount++;

      sTemp = org_str.Tokenize(_T("."), curPos);
      sTemp.Trim();
   }

   for (int k = 0, j = 0; k < sTexts.GetCount(); k++,j++)
   {
      ret = GetTranslatedString(sTexts[k],_T("en"), m_sToLangCode, sTemp);
      if(ret == FALSE)
      {
         sErrorMsg = _T("Error Translating some sentences");
         sTemp = sTexts[k];
      }
      trans_text += sTemp;
      if(j < tokenCount)
         trans_text += _T(". ");
   }

   USES_CONVERSION;
   char * temp = T2A(trans_text);
   trans_str = ToUnicodeText(temp);

   return 1;
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
   // 1. InternetOpen
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

int CTranslator::GetTranslatedString(const CString& orig_str, __in const CString& fromLang, __in const CString& ToLang, __out CString& translated_str)
{
   CString    lpszUrl;
   CString    lpszHeaders;
   DWORD    dwHeadersLength = 0;
   DWORD_PTR dwContext = 0;

   translated_str.Empty();

   CString encoded_org_str;

   EncodeStringToURLFormat(orig_str, encoded_org_str);

   lpszUrl.Format(_T("https://translate.google.com/translate_a/single?client=t&sl=%s&tl=%s&dt=t&q=%s"), fromLang, ToLang, encoded_org_str);

   lpszHeaders += _T("Host: translate.google.com\r\n");
   lpszHeaders += _T("Referer: https://translate.google.co.in/\r\n");
   lpszHeaders += _T("User-Agent: Mozilla/5.0\r\n");
   lpszHeaders += _T("Connection: close\r\n");
   lpszHeaders += _T("\r\n\r\n");

   dwHeadersLength = lpszHeaders.GetLength();

   const UINT dwFlags =
      (
      INTERNET_FLAG_KEEP_CONNECTION |
      INTERNET_FLAG_NO_CACHE_WRITE |
      INTERNET_FLAG_NO_COOKIES |
      INTERNET_FLAG_NO_UI |
      INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS
      );

   HINTERNET hOpenUrl = InternetOpenUrl(m_hInternet, lpszUrl, lpszHeaders, dwHeadersLength, dwFlags, dwContext);

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
      if(!dwBytesAvailable)
         break;

      char * temp = (char *)malloc(dwBytesAvailable*5);

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
      CString stemp(pMessageBody);
      stemp.Trim();

      int ret = stemp.Find(orig_str);
      if(ret != -1)
      {
         for (int i = 4; i < ret - 3; i++)
         {
            translated_str.AppendChar(stemp[i]);
            isTranslated = TRUE;
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
   return isTranslated;
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
   int count = sizeof(_GoogleLanguageCodeInfo) / sizeof(LanguageCode);
   for (int i = 0 ; i < count; i++)
   {
      CString stemp(_GoogleLanguageCodeInfo[i].sLanguage);
      sLanguages.Add(stemp);
   }
}

int CTranslator::GetLanguageCodeGivenLanguage(const CString& sLanguage, CString& sLangCode)
{
   int count = sizeof(_GoogleLanguageCodeInfo) / sizeof(LanguageCode);

   for(int i = 0; i < count; i++)
   {
      if(_GoogleLanguageCodeInfo[i].sLanguage == sLanguage)
      {
         sLangCode = _GoogleLanguageCodeInfo[i].sLanguageCode;
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