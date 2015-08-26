

#ifndef UNRAR_H
#define UNRAR_H

class Unrar
{
private:


    struct RAROpenArchiveData
    {
      char         *ArcName;
      unsigned int OpenMode;
      unsigned int OpenResult;
      char         *CmtBuf;
      unsigned int CmtBufSize;
      unsigned int CmtSize;
      unsigned int CmtState;
    };

    struct RARHeaderData
    {
      char         ArcName[260];
      char         FileName[260];
      unsigned int Flags;
      unsigned int PackSize;
      unsigned int UnpSize;
      unsigned int HostOS;
      unsigned int FileCRC;
      unsigned int FileTime;
      unsigned int UnpVer;
      unsigned int Method;
      unsigned int FileAttr;
      char         *CmtBuf;
      unsigned int CmtBufSize;
      unsigned int CmtSize;
      unsigned int CmtState;
    };

    struct RARHeaderDataEx
    {
      char         ArcName[1024];
      wchar_t      ArcNameW[1024];
      char         FileName[1024];
      wchar_t      FileNameW[1024];
      unsigned int Flags;
      unsigned int PackSize;
      unsigned int PackSizeHigh;
      unsigned int UnpSize;
      unsigned int UnpSizeHigh;
      unsigned int HostOS;
      unsigned int FileCRC;
      unsigned int FileTime;
      unsigned int UnpVer;
      unsigned int Method;
      unsigned int FileAttr;
      char         *CmtBuf;
      unsigned int CmtBufSize;
      unsigned int CmtSize;
      unsigned int CmtState;
      unsigned int Reserved[1024];
    };

    struct RAROpenArchiveDataEx
    {
      char         *ArcName;
      wchar_t      *ArcNameW;
      unsigned int OpenMode;
      unsigned int OpenResult;
      char         *CmtBuf;
      unsigned int CmtBufSize;
      unsigned int CmtSize;
      unsigned int CmtState;
      unsigned int Flags;
      unsigned int Reserved[32];
    };

    typedef HANDLE (PASCAL *RAROPENARCHIVE)(
        struct RAROpenArchiveData *ArchiveData
        );
    typedef HANDLE (PASCAL *RAROPENARCHIVEEX)(
        struct RAROpenArchiveDataEx *ArchiveData
        );
    typedef int (PASCAL *RARCLOSEARCHIVE)( HANDLE hArcData );
    typedef int (PASCAL *RARREADHEADER)(
        HANDLE hArcData,
        struct RARHeaderData *HeaderData
        );
    typedef int (PASCAL *RARREADHEADEREX)(
        HANDLE hArcData,
        struct RARHeaderDataEx *HeaderData
        );
    typedef int (PASCAL *RARPROCESSFILE)(
        HANDLE hArcData,
        int Operation,
        char *DestPath,
        char *DestName
        );
    typedef int (PASCAL *RARPROCESSFILEW)(
        HANDLE hArcData,
        int Operation,
        wchar_t *DestPath,
        wchar_t *DestName
        );

    // сообщения в CALLBACK - функции
    enum UNRARCALLBACK_MESSAGES {
            UCM_CHANGEVOLUME,
            UCM_PROCESSDATA,
            UCM_NEEDPASSWORD   // требуется пароль
        };
    typedef int (CALLBACK *UNRARCALLBACK)(
        UINT msg, LONG UserData, LONG P1, LONG P2
        );
    typedef void (PASCAL *RARSETCALLBACK)(
        HANDLE hArcData,
        UNRARCALLBACK Callback,
        LONG UserData
        );

    typedef int (PASCAL *CHANGEVOLPROC)(char *ArcName,int Mode);
    typedef void (PASCAL *RARSETCHANGEVOLPROC)(
        HANDLE hArcData,
        CHANGEVOLPROC ChangeVolProc
        );

    typedef int (PASCAL *PROCESSDATAPROC)(unsigned char *Addr,int Size);
    typedef void (PASCAL *RARSETPROCESSDATAPROC)(
        HANDLE hArcData,
        PROCESSDATAPROC ProcessDataProc
        );
    typedef void (PASCAL *RARSETPASSWORD)(
        HANDLE hArcData,
        char *Password
        );

    HINSTANCE m_hLib;
    RAROPENARCHIVEEX RAROpenArchiveEx;
    RARREADHEADER RARReadHeader;
    RARPROCESSFILE RARProcessFile;
    RARCLOSEARCHIVE RARCloseArchive;
    RARSETPASSWORD RARSetPassword;
    RARSETCALLBACK RARSetCallback;

    char m_sOutDir[MAX_PATH];

    void Init();
    void SetOutDir(const char *sDir);
    static int CALLBACK CallBackProc(UINT msg, LONG UserData, LONG P1, LONG P2);

public:
    Unrar();
    virtual ~Unrar();

    void Extract(const char *sArchive, const char *sDir = NULL, const char *sPass = NULL);

};

#endif

