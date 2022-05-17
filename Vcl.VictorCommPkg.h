/***************************************************************************\
*                                                                           *
*            TVictorCommPkg.h - 串行口通讯数据包协议, 版本 1.5.0.5          *
*                             Built: Jul. 10, 2015                          *
*                     Copyright © 1997-2015, Victor Chen                    *
*                      Homepage: http://www.cppfans.com                     *
*                          Email: victor@cppfans.com                        *
*                                                                           *
\***************************************************************************/

#ifndef Vcl_VictorCommPkgH
#define Vcl_VictorCommPkgH

#include "Vcl.VictorComm.h"
//---------------------------------------------------------------------------
namespace Vcl {
namespace VictorComm {
//---------------------------------------------------------------------------

#pragma pack(push,1) // { //new pack
typedef struct
 {
   unsigned long Length;  //Package Size
   unsigned char Data[1]; //All Package Data
 } TCommPackageHead;
#pragma pack(pop) // } //end of new pack
//---------------------------------------------------------------------------

enum TCommPackageType
{
  cptCompatiblePkg, //兼容以前版本 (用第一个成员是32位整数表示字节数的结构)
  cptFrameHeadTail, //用帧首尾和控制符的数据包   (帧首+带控制符的数据+帧尾)
  cptFrameTimeout , //用定义超时时间的方法判断数据包 (适合单片机的简单协议)
};
//---------------------------------------------------------------------------

class PACKAGE TCommPackageQueue
{
  public:
    __property bool Valid = { read = fIsValid };

    __property unsigned long Count   = { read = fGetCount  };
    __property unsigned long Remain  = { read = fGetRemain };
    __property unsigned long QSize   = { read = _QSize  , write = fSetQSize };
    __property unsigned long PkgSize = { read = _PkgSize, write = fSetPkgSz };

    long __fastcall In(const void *lpPkg);
    long __fastcall Out(void *lpPkg);
    long __fastcall Read(void *lpPkg, unsigned long iPkgNo);
    void __fastcall SetQueue(long QSz, long PkgSz);

    void __fastcall Clear(void);

    __fastcall TCommPackageQueue(unsigned long QSz, unsigned long PkgSz);
    virtual __fastcall ~TCommPackageQueue();

  private:
    char *_QBuffer;
    unsigned long _Head, _Tail, _QSize, _PkgSize;

    unsigned long __fastcall fGetCount(void)const;
    unsigned long __fastcall fGetRemain(void)const;

    bool __fastcall fIsValid(void)const;

    void __fastcall fRemoveQueue(void);

    void __fastcall fSetQSize(unsigned long qs);
    void __fastcall fSetPkgSz(unsigned long ps);
};
//---------------------------------------------------------------------------

class PACKAGE TCommPackage:public TObject
{
  public:
    static const int DefaultFrameHead = 0xeb;
    static const int DefaultFrameTail = 0xee;
    static const int DefaultFrameCtrl = 0xec;

  public:
    __fastcall TCommPackage(TVictorComm *lpComm);
    virtual __fastcall ~TCommPackage();

    __property TVictorComm *CommPort = { read = _CommPort                    };
    __property bool         Active   = { read = _bActive, write = fSetActive };

    __property unsigned long    QueueSize       = { read = _QueueSize       , write = fSetQSize        , default =    16 };
    __property unsigned long    PackageSize     = { read = _PackageDataSize , write = fSetPkgSz        , default =  4096 };
    __property TCommPackageType PackageType     = { read = _PackageType     , write = fSetPkgType      , default = cptFrameTimeout };
    __property DWORD            PackageInterval = { read = _dwTimerInterval , write = fSetPkgInterval  , default =    50 }; // 50ms
    __property bool             DoubleFrameMark = { read = _bDoubleFrameMark, write = fSetDblFrameMark , default = false };

    __property unsigned char FrameHead = { read = _FRAMEHEAD, write = _FRAMEHEAD , default = DefaultFrameHead }; //0xeb //eb=Begin
    __property unsigned char FrameTail = { read = _FRAMETAIL, write = _FRAMETAIL , default = DefaultFrameTail }; //0xee //ee=End
    __property unsigned char FrameCtrl = { read = _FRAMECTRL, write = _FRAMECTRL , default = DefaultFrameCtrl }; //0xec //ec=Control

    __property TCommPackageQueue *QueueRead  = { read = _QueueRead  };
    __property TCommPackageQueue *QueueWrite = { read = _QueueWrite };

    __property void __fastcall (__closure *OnCommNotify) (TObject *Sender, int NotifyType) = { read = lpUserCommNotify, write = lpUserCommNotify };

    virtual unsigned long __fastcall ReadPackage(void *lpPkg, unsigned long BufSize);
    virtual unsigned long __fastcall WritePackage(const void *lpPkg, unsigned long nBytes);

    virtual void __fastcall PurgeRead(void);
    virtual void __fastcall PurgeWrite(void);

  protected:
    virtual void __fastcall CallNotify(int NotifyType);

  private:
    bool _bActive, _bDoubleFrameMark;
    void __fastcall fSetActive(bool b);

    bool _bFrameHeadDetected, _bControlDetected;
    unsigned long _BufferBytesWrite;

    TVictorComm *_CommPort;
    void __fastcall fNewCommNotify(TObject *Sender, int NotifyType);
    void __fastcall (__closure *lpOldCommNotify)(TObject *Sender, int NotifyType);
    void __fastcall (__closure *lpUserCommNotify)(TObject *Sender, int NotifyType);

    TCommPackageQueue *_QueueRead, *_QueueWrite;
    TCommPackageHead *_BufferRead, *_BufferWrite, *_BufferTemp;

    void __fastcall _AllocBuffer(void);
    void __fastcall _RemoveBuffer(void);

    unsigned long    _QueueSize;        //default: 16    (缓存最多能够保存的数据包个数)
    unsigned long    _PackageDataSize;  //defalut: 4096  (数据包数据的最大长度，不包括表示长度的32位整数)
    TCommPackageType _PackageType;      //default: cptFrameTimeout (时间间隔判断数据包)

    void fSetQSize(unsigned long);
    void fSetPkgSz(unsigned long);
    void fSetPkgType(TCommPackageType);
    void fSetPkgInterval(DWORD);
    void fSetDblFrameMark(bool);

    unsigned char _FRAMEHEAD; //0xeb //eb=Begin
    unsigned char _FRAMETAIL; //0xee //ee=End
    unsigned char _FRAMECTRL; //0xec //ec=Control

    unsigned long __fastcall _EncodePackage(TCommPackageHead *lpEncoded, TCommPackageHead *lpPackage); //编码，目标包括头、尾
    void __fastcall _ReadHeadTailPackage(void);
    bool __fastcall _ReadTimeoutPackage(void);
    void __fastcall _WritePackage(void);
    bool _bWritingPackage;

    void __fastcall _TimerThread(void);
    DWORD _dwTimerInterval;
    long _hTimerThreadId, _iSkipTimerEvents;
    bool _bRunTimerThread, _bTimerThreadRunning;
    HANDLE _hTimerEvent;
    static void _USERENTRY _fnTimerThread(void *);
};
//---------------------------------------------------------------------------
} // namespace VictorComm
} // namespace Vcl
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
