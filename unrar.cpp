#include "unrar.h"

//---------------------------------------------------------------------------

namespace
{
    #define ERAR_END_ARCHIVE     10
    #define ERAR_NO_MEMORY       11
    #define ERAR_BAD_DATA        12
    #define ERAR_BAD_ARCHIVE     13
    #define ERAR_UNKNOWN_FORMAT  14
    #define ERAR_EOPEN           15
    #define ERAR_ECREATE         16
    #define ERAR_ECLOSE          17
    #define ERAR_EREAD           18
    #define ERAR_EWRITE          19
    #define ERAR_SMALL_BUF       20
    #define ERAR_UNKNOWN         21

    #define RAR_OM_LIST           0
    #define RAR_OM_EXTRACT        1

    #define RAR_SKIP              0
    #define RAR_TEST              1
    #define RAR_EXTRACT           2

    #define RAR_VOL_ASK           0
    #define RAR_VOL_NOTIFY        1

    const char RAROpenArchiveExName[] = "RAROpenArchiveEx";
    const char RARReadHeaderName[]    = "RARReadHeader";
    const char RARProcessFileName[]   = "RARProcessFile";
    const char RARCloseArchiveName[]  = "RARCloseArchive";
    const char RARSetPasswordName[]   = "RARSetPassword";
    const char RARSetCallbackName[]   = "RARSetCallback";
}
//---------------------------------------------------------------------------

Unrar::Unrar() : RAROpenArchiveEx(NULL), RARReadHeader(NULL),
                 RARProcessFile(NULL), RARCloseArchive(NULL),
                 RARSetPassword(NULL),
                 m_hLib(NULL)
{
    Init();
}
//---------------------------------------------------------------------------

Unrar::~Unrar()
{
    if(m_hLib)
        FreeLibrary(m_hLib);
}
//---------------------------------------------------------------------------
//
// IN: sArchive - маска файлов, sDir - каталог извлечения, sPass - пароль
void Unrar::Extract(const char *sArchive, const char *sDir, const char *sPass)
{
    if(!m_hLib)
        return;

    char str[MAX_PATH], tmp[MAX_PATH];

    // здесь будет каталог расположения архивов
    lstrcpy(tmp, sArchive);
    int i;
    for(i = lstrlen(tmp) - 1; i >=0; i--)
        if(tmp[i] == '\\')
        {
            tmp[i + 1] = 0;
            break;
        }
    if(i < 0)
        tmp[0] = 0;

    SetOutDir(sDir);

    WIN32_FIND_DATA FindData;
    HANDLE hFind = FindFirstFile(sArchive, &FindData);
    if(hFind != INVALID_HANDLE_VALUE)
    {
        HANDLE hArcData;
        int RHCode, PFCode;
        struct RARHeaderData HeaderData;
        struct RAROpenArchiveDataEx OpenArchiveData;
        char sCmtBuf[16*1024];

        do
        {
            wsprintf(str, "%s%s", tmp, FindData.cFileName);

            ZeroMemory(&OpenArchiveData, sizeof(OpenArchiveData));
            OpenArchiveData.ArcName = str;
            OpenArchiveData.CmtBuf  = sCmtBuf;
            OpenArchiveData.CmtBufSize = sizeof(sCmtBuf);
            OpenArchiveData.OpenMode = RAR_OM_EXTRACT;

            hArcData = RAROpenArchiveEx(&OpenArchiveData);

            if( !OpenArchiveData.OpenResult )
            {
                // передаем параметром в Callback пароль
                RARSetCallback(hArcData, CallBackProc, (LONG)sPass);
                HeaderData.CmtBuf = NULL;

                while( (RHCode = RARReadHeader(hArcData, &HeaderData)) == 0 )
                {
                    PFCode = RARProcessFile(hArcData, RAR_EXTRACT, m_sOutDir, NULL);
                    if(PFCode)
                        break;
                }

//
//                if(RHCode == ERAR_BAD_DATA)
//                {
//                    wsprintf(str, "Ошибка архива: %s", FindData.cFileName);
//                    MessageBox(GetActiveWindow(), str, "Unrar", MB_OK|MB_ICONERROR);
//                }

                RARCloseArchive(hArcData);
            }
        } while( FindNextFile(hFind, &FindData) );
        FindClose(hFind);
    }
}
//---------------------------------------------------------------------------

// библиотека UNRAR требует имена каталогов в OEM кодировке
void Unrar::SetOutDir(const char *sDir)
{
    CharToOem(sDir, m_sOutDir);
}
//---------------------------------------------------------------------------

// загружаем библиотеку и инициализируем функции
void Unrar::Init()
{
    m_hLib = LoadLibrary("UNRAR.DLL");
    if(m_hLib)
    {
        RAROpenArchiveEx = (RAROPENARCHIVEEX) GetProcAddress(m_hLib, RAROpenArchiveExName);
        RARReadHeader    = (RARREADHEADER) GetProcAddress(m_hLib, RARReadHeaderName);
        RARProcessFile   = (RARPROCESSFILE) GetProcAddress(m_hLib, RARProcessFileName);
        RARCloseArchive  = (RARCLOSEARCHIVE) GetProcAddress(m_hLib, RARCloseArchiveName);
        RARSetCallback   = (RARSETCALLBACK) GetProcAddress(m_hLib, RARSetCallbackName);

        RARSetPassword   = (RARSETPASSWORD) GetProcAddress(m_hLib, RARSetPasswordName);

        // проверка на инициализацию необходимых функций
        if( !RAROpenArchiveEx || !RARReadHeader ||
            !RARProcessFile || !RARCloseArchive || !RARSetCallback )
        {
            FreeLibrary(m_hLib);
            m_hLib = NULL;
        }
    }
}
//---------------------------------------------------------------------------

int CALLBACK Unrar::CallBackProc(UINT msg, LONG UserData, LONG P1, LONG P2)
{
    switch(msg)
    {
    case UCM_NEEDPASSWORD:
        // в UserData переданные параметром в CallBack данные
        // в нашем случае - пароль
        lstrcpyn((char *)P1, (char *)UserData, P2);
        return 0;
    }
    return 0;
}
