//---------------------------------------------------------------------------
//               TVictorComm.h - 串行口通讯程序, 版本 1.5.0.5              //
//                           Built: Jul. 10, 2015                          //
//                    Copyright © 1997-2015, Victor Chen                   //
//                     Homepage: http://www.cppfans.com                    //
//                         Email: victor@cppfans.com                       //
//---------------------------------------------------------------------------
#ifndef Vcl_VictorCommH
#define Vcl_VictorCommH
//---------------------------------------------------------------------------
#include <vcl.h>
#include <tchar.h>
#include "Vcl.VictorEnumSerial.h"
//---------------------------------------------------------------------------
namespace Vcl {
namespace VictorComm {
//---------------------------------------------------------------------------
class PACKAGE TCommQueue
{
public:
  TCommQueue(long lSize=8192);
  virtual ~TCommQueue();

  long In(const char far *s);         //RetVal: num of chars pushed in
  long In(const char far *s, long n); //RetVal: num of chars pushed in
  long Out(char far *s);              //RetVal: num of chars poped out
  long Out(char far *s, long n);      //RetVal: num of chars poped out
  void Clear(void);

  __property long Count  = { read = fGetCount  }; //readonly property
  __property long QSize  = { read = _BufSize, write = fSetBufSize };
  __property bool Valid  = { read = fIsValid   }; //readonly property
  __property long Remain = { read = fGetRemain }; //readonly property

protected:
  bool fIsValid(void)const;
  long fGetCount(void)const;
  long fGetRemain(void)const;
  void fSetBufSize(long n);
  long _BufSize, _Head, _Tail;
  char *_Buffer;
};
//---------------------------------------------------------------------------
class PACKAGE TVictorComm : public TObject
{
public:
  enum TFlowControl { fcNone = 0, fcRtsCts = 3, fcXonXoff = 4, fcTranXonXoff = 5, fcRtsCtsXonXoff = 6 }; //AT&Kn

public:
  __fastcall TVictorComm();
  virtual __fastcall ~TVictorComm();

  __property int           PortNo   = { read = _PortNo   , write = fSetPort     };
  __property UnicodeString PortName = { read = _PortName , write = fSetPortName };
  __property HANDLE        Handle   = { read = _Handle   , write = fSetHandle   };
  __property bool          Active   = { read = _Opened   , write = fSetActive   };

  __property DWORD  Baud     = { read = fGetBaud    , write = fSetBaud    };
  __property BYTE   Parity   = { read = fGetParity  , write = fSetParity  };
  __property BYTE   ByteSize = { read = fGetByteSize, write = fSetByteSize};
  __property BYTE   StopBits = { read = fGetStopBits, write = fSetStopBits};

  __property long InBufSize  = { read = fGetInBufSize , write = fSetInBufSize };
  __property long OutBufSize = { read = fGetOutBufSize, write = fSetOutBufSize};

  __property long HwInSize   = { read = _HwInSize , write = fSetHwInSize  };
  __property long HwOutSize  = { read = _HwOutSize, write = fSetHwOutSize };

  __property bool DataOnly   = { read = _bDataOnly, write = fSetDataOnly  };

  virtual long __fastcall Read(void far *s, long n);
  virtual long __fastcall Write(const void far *s, long n);
  virtual long __fastcall Command(const char far *s);

  virtual void __fastcall PurgeRead(bool bAbort=false);
  virtual void __fastcall PurgeWrite(bool bAbort=false);

  __property TFlowControl  FlowControl = { read = fGetFCtrl, write = fSetFCtrl };
  __property unsigned char AutoAnswer  = { read = _AutoAns , write = fSetAAns  };
  __property DWORD         ModemStatus = { read = _ModemStatus }; //MS_CTS_ON | MS_DSR_ON | MS_RING_ON | MS_RLSD_ON

  __property bool DTR  = { read = fGetDTR , write = fSetDTR  }; //Computer status, read/write
  __property bool RTS  = { read = fGetRTS , write = fSetRTS  }; //Computer status, read/write
  __property bool CTS  = { read = fGetCTS  }; //Modem status, read-only
  __property bool DSR  = { read = fGetDSR  }; //Modem status, read-only
  __property bool RING = { read = fGetRING }; //Modem status, read-only
  __property bool RLSD = { read = fGetRLSD }; //Modem status, read-only

  virtual void __fastcall ResetModem(void); //reset modem to power-on status
  virtual void __fastcall InitModem(void); //init modem using TVictorComm properties
  virtual BOOL __fastcall EscapeCommFunction(DWORD dwFunc); //dwFunc: one of the following values: CLRDTR,CLRRTS,SETDTR,SETRTS,SETXOFF,SETXON,SETBREAK,CLRBREAK

  __property bool  FromHandle   = { read = _bFromHandle };
  __property bool  DisableWrite = { read = _DisableWrite, write = _DisableWrite};

  __property TCommQueue  *InQueue  = { read = _InQueue    };
  __property TCommQueue  *OutQueue = { read = _OutQueue   };

  //可以产生 OnCommNotify 的事件: EV_RXCHAR|EV_TXEMPTY|EV_CTS|EV_DSR|EV_RLSD|EV_RING|EV_RXFLAG|EV_RX80FULL|EV_ERR
  __property void __fastcall (__closure *OnCommNotify) (TObject *Sender, int NotifyType) = { read = lpCommNotify, write=lpCommNotify };
  __property void __fastcall (__closure *AfterOpen) (TObject *Sender) = { read = lpAfterOpen, write = lpAfterOpen };
  __property void __fastcall (__closure *BeforeClose) (TObject *Sender) = { read = lpBeforeClose, write = lpBeforeClose };

protected:
  virtual void __fastcall Open(void);
  virtual void __fastcall Close(void);

  virtual void __fastcall CommNotify(int NotifyType); //EV_RXCHAR or EV_TXEMPTY
  virtual void __fastcall CommAfterOpen(void);
  virtual void __fastcall CommBeforeClose(void);

private:
  void __fastcall fSetPort(int COMn);
  void __fastcall fSetPortName(UnicodeString s);
  void __fastcall fSetHandle(HANDLE hFile);

  void __fastcall fSetActive(bool bActive);
  void __fastcall fSetDataOnly(bool bDO);

  DWORD __fastcall fGetBaud    (void)const;
  BYTE  __fastcall fGetParity  (void)const;
  BYTE  __fastcall fGetByteSize(void)const;
  BYTE  __fastcall fGetStopBits(void)const;

  void __fastcall fSetBaud    (DWORD NewBaud  );
  void __fastcall fSetParity  (BYTE  NewParity);
  void __fastcall fSetByteSize(BYTE  NewBSize );
  void __fastcall fSetStopBits(BYTE  NewSBits );

  long __fastcall fGetInBufSize (void)const;
  long __fastcall fGetOutBufSize(void)const;
  void __fastcall fSetInBufSize (long n);
  void __fastcall fSetOutBufSize(long n);

  void __fastcall fSetHwBufSize(long HwIn, long HwOut);
  void __fastcall fSetHwInSize (long NewSize);
  void __fastcall fSetHwOutSize(long NewSize);

  TFlowControl __fastcall fGetFCtrl(void);
  void         __fastcall fSetFCtrl(TFlowControl);
  void         __fastcall fSetAAns(unsigned char aans);

  bool __fastcall fGetDTR(void)const; // Computer status, read/write
  bool __fastcall fGetRTS(void)const; // Computer status, read/write
  void __fastcall fSetDTR(bool);      // Computer status, read/write
  void __fastcall fSetRTS(bool);      // Computer status, read/write

  bool __fastcall fGetCTS (void)const; // Modem status, read-only
  bool __fastcall fGetDSR (void)const; // Modem status, read-only
  bool __fastcall fGetRING(void)const; // Modem status, read-only
  bool __fastcall fGetRLSD(void)const; // Modem status, read-only

private:
  HANDLE _Handle;    //INVALID_HANDLE_VALUE
  DCB    _dcb;
  bool   _bFromHandle, _Opened, _DisableWrite;
  bool   _bDTR, _bRTS;
  bool   _bDataOnly;

  int           _PortNo;
  UnicodeString _PortName;
  long          _HwInSize, _HwOutSize;
  unsigned char _AutoAns;

  TCommQueue  *_InQueue;
  TCommQueue  *_OutQueue;

  static const char  Fmt_InitModem[];  //"ATE0&K%dS0=%d\r"
  static const char  Str_ResetModem[]; //"ATZ\r"

  void __fastcall (__closure *lpCommNotify) (TObject *Sender, int NotifyType);
  void __fastcall (__closure *lpAfterOpen) (TObject *Sender);
  void __fastcall (__closure *lpBeforeClose) (TObject *Sender);

  static void _ReadThread(void *Param);
  static void _WriteThread(void *Param);

private:
  volatile uintptr_t _ReadThreadId;       //-1
  volatile bool      _RunReadThread;      // 0
  volatile bool      _ReadThreadRunning;  // 0

  volatile uintptr_t _WriteThreadId;      //-1
  volatile bool      _RunWriteThread;     // 0
  volatile bool      _WriteThreadRunning; // 0

  OVERLAPPED    _ReadOS;             //
  OVERLAPPED    _WriteOS;            //write overlapped structure

  HANDLE        _hKillRead;          //kill read thread //NULL
  HANDLE        _hKillWrite;         //kill write thread //NULL
  HANDLE        _hSyncWrite;         //write synchronize event //NULL

  DWORD         _ModemStatus;        //MS_CTS_ON | MS_DSR_ON | MS_RING_ON | MS_RLSD_ON
};
//---------------------------------------------------------------------------
class PACKAGE TVictorCommLanguage
{
public:
  enum TSysLCIDs
   {
     lcidNeutral     = MAKELANGID(LANG_NEUTRAL,SUBLANG_NEUTRAL    ), //Language neutral
     lcidUserDefault = MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT    ), //User default language
     lcidSysDefault  = MAKELANGID(LANG_NEUTRAL,SUBLANG_SYS_DEFAULT), //System default language

     lcidEnu = MAKELANGID(LANG_ENGLISH,SUBLANG_ENGLISH_US         ), //United States English
     lcidCht = MAKELANGID(LANG_CHINESE,SUBLANG_CHINESE_TRADITIONAL), //Traditional Chinese
     lcidChs = MAKELANGID(LANG_CHINESE,SUBLANG_CHINESE_SIMPLIFIED ), //Simplified Chinese
   };

  __property LCID lcId = { read = _lcId, write = fSetLcId };

  TVictorCommLanguage();
private:
  LCID _lcId;
  void fSetLcId(LCID id);
};
//---------------------------------------------------------------------------
class PACKAGE EVictorCommError : public Exception
{
public:
 enum TErrorCode
  {
    COMM_NOERROR   =  0, //没有错误
    COMM_NOMEMORY  =  1, //内存不够
    COMM_INITRDBUF =  2, //不能初始化读缓存
    COMM_INITWRBUF =  3, //不能初始化写缓存
    COMM_OPENPORT  =  4, //不能打开端口
    COMM_SETMASK   =  5, //不能设置端口事件MASK
    COMM_BUFSIZE   =  6, //不能设置端口缓存
    COMM_GETSTATE  =  7, //不能得到端口参数
    COMM_SETSTATE  =  8, //不能设置端口参数
    COMM_CRRDEVENT =  9, //不能创建端口异步读事件
    COMM_CRWREVENT = 10, //不能创建端口异步写事件
    COMM_CRRDTHREAD= 11, //不能创建端口读线程
    COMM_CRWRTHREAD= 12, //不能创建端口写线程
    COMM_CLOSERDT  = 13, //不能关闭端口读线程
    COMM_CLOSEWRT  = 14, //不能关闭端口写线程
  };

 __property TErrorCode ErrorCode = { read = _ErrorCode };
 __property UnicodeString PortName = { read = _PortName };

 __fastcall EVictorCommError(TErrorCode ecErrCode);
 __fastcall EVictorCommError(TErrorCode ecErrCode, UnicodeString sPortName);

protected:
 TErrorCode _ErrorCode;
 UnicodeString _PortName;

private:
 class MessageStrings
  {
	public:
	  typedef struct { TErrorCode Code; const wchar_t *Msg; } ERRMSG;
	  __property UnicodeString ErrMsg[TErrorCode] = { read = fGetErrMsg };
	  __fastcall MessageStrings(UnicodeString sPortName);

	private:
	  UnicodeString __fastcall fGetErrMsg(TErrorCode ecCode);

	  ERRMSG  *Err_Messages;
	  wchar_t *Err_PortName;
	  wchar_t *Err_Unknown;

	  static ERRMSG  Err_Messages_Chs[], Err_Messages_Cht[], Err_Messages_Enu[];
	  static wchar_t Err_PortName_Chs[], Err_PortName_Cht[], Err_PortName_Enu[];
	  static wchar_t Err_Unknown_Chs [], Err_Unknown_Cht [], Err_Unknown_Enu [];

	  UnicodeString _PortName;
	  TVictorCommLanguage _CommLanguage;
  };
};
//---------------------------------------------------------------------------
} // namespace VictorComm
} // namespace Vcl
//---------------------------------------------------------------------------
using namespace Vcl::VictorComm;
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
