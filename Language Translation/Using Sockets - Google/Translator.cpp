#include "StdAfx.h"
#include "Translator.h"
#include <WinSock2.h>
#include <WS2tcpip.h>

#define RECIEVE_BUFFER_SIZE 5000

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
   m_sToLangCode = _T("kn");
   m_clientSocket = INVALID_SOCKET;
   m_sErrorMsg.Empty();

   LoadSocketDllsandLibs();
}

CTranslator::~CTranslator(void)
{
   WSACleanup();
   closesocket(m_clientSocket);
}

int CTranslator::LoadSocketDllsandLibs()
{
   WORD wVersionRequested;
   WSADATA wsaData;
   int wsaerr;

   // Using MAKEWORD macro, Winsock version request 2.2
   wVersionRequested = MAKEWORD(2, 2);

   wsaerr = WSAStartup(wVersionRequested, &wsaData);
   if (wsaerr != 0)
   {
      /* Tell the user that we could not find a usable */
      /* WinSock DLL.*/
      m_sErrorMsg = _T("The Winsock dll not found!\n");
      return 0;
   }
   else
   {
      printf("The Winsock dll found!\n");
      printf("The status: %s.\n", wsaData.szSystemStatus);
   }

   /* Confirm that the WinSock DLL supports 2.2.*/
   /* Note that if the DLL supports versions greater    */
   /* than 2.2 in addition to 2.2, it will still return */
   /* 2.2 in wVersion since that is the version we      */
   /* requested.                                        */
   if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2 )
   {
      /* Tell the user that we could not find a usable */
      /* WinSock DLL.*/
      m_sErrorMsg.Format(_T("The dll do not support the Winsock version %u.%u!\n"), LOBYTE(wsaData.wVersion),HIBYTE(wsaData.wVersion));
      WSACleanup();
      return 0;
   }
   else
   {
      printf("The dll supports the Winsock version %u.%u!\n", LOBYTE(wsaData.wVersion),HIBYTE(wsaData.wVersion));
      printf("The highest version this dll can support: %u.%u\n", LOBYTE(wsaData.wHighVersion), HIBYTE(wsaData.wHighVersion));
   }
   return 1;
}

int CTranslator::CreateAndConnectSocket()
{
   struct addrinfo host_info;       // The struct that getaddrinfo() fills up with data.
   struct addrinfo *host_info_list; // Pointer to the to the linked list of host_info's.

   memset(&host_info, 0, sizeof host_info);

   host_info.ai_family = AF_UNSPEC;     // IP version not specified. Can be both.
   host_info.ai_socktype = SOCK_STREAM; // Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.

   int status = getaddrinfo("translate.google.com", "80", &host_info, &host_info_list);

   if(status != 0)
   {
     m_sErrorMsg = _T("Couldn't connect to Internet!");
     return 0;
   }

   /* 1. Socket Creation. */
   int family = host_info_list->ai_family;
   int connection_type = host_info_list->ai_socktype;
   int protocol = host_info_list->ai_protocol;

   m_clientSocket = socket(family, connection_type, protocol);

   if(m_clientSocket == INVALID_SOCKET)
   {
      m_sErrorMsg = _T("Socket creation failed");
      return 0;
   }

   /*2. Connect to server  */
   struct sockaddr_in dest; /* socket info about our server */
   memset(&dest, 0, sizeof(dest));                /* zero the struct */
   dest.sin_family = AF_INET;
   dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK); /* set destination IP number - localhost, 127.0.0.1*/ 
   dest.sin_port = htons(2300);                /* set destination port number */

   //status = connect(m_clientSocket, (struct sockaddr *)&dest, sizeof(struct sockaddr));
   status = connect(m_clientSocket, host_info_list->ai_addr, host_info_list->ai_addrlen);
   if(status == -1)
   {
      m_sErrorMsg = _T("Socket Connecting failed");
      closesocket(m_clientSocket);
      m_clientSocket = INVALID_SOCKET;
      return 0;
   }

   freeaddrinfo(host_info_list);
   return 1;
}

int CTranslator::GetTranslation(CString& org_str, CString& trans_str, CString& sErrorMsg)
{
   CString      sTemp, trans_text ;
   CStringArray sTexts;
   int curPos = 0, tokenCount = 0, ret = 0;

   sErrorMsg.Empty();

   if(m_clientSocket == INVALID_SOCKET)
   {
      ret = CreateAndConnectSocket();
      if(ret == FALSE)
      {
         sErrorMsg = m_sErrorMsg;
         trans_text.Empty();
         return 0;
      }
   }

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
         sErrorMsg = _T("Error Translating sentence");
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

int CTranslator::GetTranslatedString(const CString& orig_str, __in const CString& fromLang, __in const CString& ToLang, __out CString& translated_str)
{
   translated_str.Empty();

   CString encoded_org_str;

   EncodeStringToURLFormat(orig_str, encoded_org_str);
   CString url;

   url.Format(_T("GET /translate_a/single?client=j&sl=%s&tl=%s&dt=t&ie=UTF-8&oe=UTF-8&q=%s "), fromLang, ToLang, encoded_org_str);

   url += _T("HTTP/1.1\r\n");
   url += _T("Host: translate.google.com\r\n");
   url += _T("\r\n\r\n");

   USES_CONVERSION;
   char * msg = T2A(url);

   int len;
   int bytes_sent;
   len = strlen(msg);
   if(m_clientSocket != INVALID_SOCKET)
      bytes_sent = send(m_clientSocket, msg, len, 0);

   // Receive

   int  bytes_recieved = 0;
   char incoming_data_buffer[RECIEVE_BUFFER_SIZE+1];
   CString sMessageBody;
   if(m_clientSocket != INVALID_SOCKET)
   {
      while(1)
      {
         bytes_recieved = recv(m_clientSocket, incoming_data_buffer,RECIEVE_BUFFER_SIZE, 0);
         if (bytes_recieved == -1)
         {
            m_sErrorMsg = _T("Socket receive error!");
            break;
         }
         if(bytes_recieved == 0)
            break;

         incoming_data_buffer[bytes_recieved] = '\0';
         sMessageBody += incoming_data_buffer;

         if(bytes_recieved < RECIEVE_BUFFER_SIZE)
            break;
      }
   }

   CString  HTTPHeaderDelimiter = _T("\r\n\r\n");

   int ret = sMessageBody.Find(HTTPHeaderDelimiter);
   if(ret == -1)
      return 0 ;

   CString stemp = sMessageBody.Right(sMessageBody.GetLength() - ret );
   stemp.Trim();

   ret = stemp.Find('[');
   if(ret)
      stemp = stemp.Right(stemp.GetLength() - ret );

   ret = stemp.Find(orig_str);

   if(ret == -1)
      return 0 ;

   for (int i = 4; i < ret - 3; i++)
   {
      translated_str.AppendChar(stemp[i]);
   }
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