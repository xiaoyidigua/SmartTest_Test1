/***************************************************************************\
*                                                                           *
*             Vcl.YbCommDevice.h - 串行口通讯控件, 版本 1.5.0.6             *
*                            Built: Apr. 17, 2017                           *
*                     Copyright © 1997-2017, Victor Chen                    *
*                      Homepage: http://www.cppfans.com                     *
*                          Email: victor@cppfans.com                        *
*                                                                           *
\***************************************************************************/

#ifndef Vcl_YbCommDeviceH
#define Vcl_YbCommDeviceH
//---------------------------------------------------------------------------
#include "Vcl.VictorCommPkg.h"
//---------------------------------------------------------------------------
namespace Vcl {
namespace VictorComm {
//---------------------------------------------------------------------------
class PACKAGE TYbCommFrameSettings: public TPersistent
{
__published:
    __property unsigned char FrameHead = { read = fGetFrameHead, write = fSetFrameHead, default = TCommPackage::DefaultFrameHead };
    __property unsigned char FrameTail = { read = fGetFrameTail, write = fSetFrameTail, default = TCommPackage::DefaultFrameTail };
    __property unsigned char FrameCtrl = { read = fGetFrameCtrl, write = fSetFrameCtrl, default = TCommPackage::DefaultFrameCtrl };
    __property bool    DoubleFrameMark = { read = fGetDblFrMark, write = fSetDblFrMark, default = false                          };

public:
    __fastcall TYbCommFrameSettings(TCommPackage *lpPkg);
    void __fastcall SetValues(TYbCommFrameSettings *lpfs);

private:
    TCommPackage *_CommPkg;

    bool __fastcall fGetDblFrMark(void)const;
    void __fastcall fSetDblFrMark(bool b);

    unsigned char __fastcall fGetFrameHead(void)const;
    unsigned char __fastcall fGetFrameTail(void)const;
    unsigned char __fastcall fGetFrameCtrl(void)const;

    void __fastcall fSetFrameHead(unsigned char c);
    void __fastcall fSetFrameTail(unsigned char c);
    void __fastcall fSetFrameCtrl(unsigned char c);
};
//---------------------------------------------------------------------------
class PACKAGE TYbCustomCommDevice : public TComponent
{
  public:
    enum TBaudRate
     {
       br110    , // CBR_110
       br300    , // CBR_300
       br600    , // CBR_600
       br1200   , // CBR_1200
       br2400   , // CBR_2400
       br4800   , // CBR_4800
       br9600   , // CBR_9600
       br14400  , // CBR_14400
       br19200  , // CBR_19200
       br38400  , // CBR_38400
       br56000  , // CBR_56000
       br57600  , // CBR_57600
       br115200 , // CBR_115200
       br128000 , // CBR_128000
       br256000 , // CBR_256000
       brCustom , // Custom Baud Rate
     };

    enum TParity
     {
       ptNoParity    , // NOPARITY
       ptOddParity   , // ODDPARITY
       ptEvenParity  , // EVENPARITY
       ptMarkParity  , // MARKPARITY
       ptSpaceParity , // SPACEPARITY
     };

    enum TStopBits
     {
       sbOneStopBit     , // ONESTOPBIT   //1 stop bit
       sbOne_5_StopBits , // ONE5STOPBITS //1.5 stop bits
       sbTwoStopBit     , // TWOSTOPBITS  //2 stop bits
     };

    enum TFlowControl
     {
       fcNone          , // AT&K0
       fcRtsCts        , // AT&K3
       fcXonXoff       , // AT&K4
       fcTranXonXoff   , // AT&K5
       fcRtsCtsXonXoff , // AT&K6
     };

    enum TModemStatusItem
     {
       msCtsOn  , // MS_CTS_ON
       msDsrOn  , // MS_DSR_ON
       msRingOn , // MS_RING_ON
       msRlsdOn , // MS_RLSD_ON
     };

    enum TNotifyMsgType
     {
       nmtCommMsg = WM_USER + 2,
       nmtPackage = WM_USER + 3,
     };

    typedef Set<TModemStatusItem,msCtsOn,msRlsdOn>TModemStatus;

    __property bool          Active      = { read = fGetActive    , write = fSetActive     };
    __property int           PortNo      = { read = fGetPort      , write = fSetPort       };
    __property UnicodeString PortName    = { read = fGetPortName  , write = fSetPortName   };
    __property UnicodeString PortParams  = { read = fGetPortParams, write = fSetPortParams };

    __property TBaudRate    Baud         = { read = fGetBaud     , write = fSetBaud     , default = br115200     };
    __property DWORD        BaudRate     = { read = fGetBaudRate , write = fSetBaudRate , default = 115200       };
    __property TParity      Parity       = { read = fGetParity   , write = fSetParity   , default = ptNoParity   };
    __property int          ByteSize     = { read = fGetByteSize , write = fSetByteSize , default = 8            };
    __property TStopBits    StopBits     = { read = fGetStopBits , write = fSetStopBits , default = sbOneStopBit };

    __property TFlowControl FlowControl  = { read = fGetFlCtrl       , write = fSetFlCtrl };
    __property int          AutoAnswer   = { read = fGetAutAns       , write = fSetAutAns };
    __property TModemStatus ModemStatus  = { read = fGetModemStatus                       }; //read only
    __property bool         DisableWrite = { read = fGetDisableWrite , write = fSetDisableWrite};
    __property bool         SyncEvents   = { read = _bSyncEvents     , write = fSetSyncEvents , default = true  };
    __property bool         DataOnly     = { read = fGetDataOnly     , write = fSetDataOnly   , default = false };

    __property long         InBufSize    = { read = fGetInBufSize  , write = fSetInBufSize  , default = 8192 };
    __property long         OutBufSize   = { read = fGetOutBufSize , write = fSetOutBufSize , default = 8192 };

    __property long         HwInSize     = { read = fGetHwInSize  , write = fSetHwInSize  , default = 1200 };
    __property long         HwOutSize    = { read = fGetHwOutSize , write = fSetHwOutSize , default = 1200 };

    __property long             QueueSize       = { read = fGetQSize      , write = fSetQSize      , default =   16 };
    __property long             PackageSize     = { read = fGetPkgSz      , write = fSetPkgSz      , default = 4096 };
    __property TCommPackageType PackageType     = { read = fGetPkgType    , write = fSetPkgType    , default = cptFrameTimeout };
    __property long             PackageInterval = { read = fGetPkgInterval, write = fSetPkgInterval, default =   50 };

    __property bool DTR  = { read = fGetDTR , write = fSetDTR  }; //Computer status, read/write
    __property bool RTS  = { read = fGetRTS , write = fSetRTS  }; //Computer status, read/write
    __property bool CTS  = { read = fGetCTS  }; //Modem status, read-only
    __property bool DSR  = { read = fGetDSR  }; //Modem status, read-only
    __property bool RING = { read = fGetRING }; //Modem status, read-only
    __property bool RLSD = { read = fGetRLSD }; //Modem status, read-only, DCD=RLSD
    __property bool DCD  = { read = fGetRLSD }; //Modem status, read-only, DCD=RLSD

    __property void __fastcall (__closure *OnPackage   ) (TObject *Sender, int NotifyType) = { read = _lpfnPackageNotify, write = _lpfnPackageNotify };
    __property void __fastcall (__closure *OnCommNotify) (TObject *Sender, int NotifyType) = { read = _lpfnCommNotify   , write = _lpfnCommNotify    };
    __property void __fastcall (__closure *AfterOpen   ) (TObject *Sender) = { read = _lpfnAfterOpen  , write = _lpfnAfterOpen   };
    __property void __fastcall (__closure *BeforeClose ) (TObject *Sender) = { read = _lpfnBeforeClose, write = _lpfnBeforeClose };

    virtual long __fastcall Read(void far *s, long n);
    virtual long __fastcall Write(const void far *s, long n);
    virtual long __fastcall Command(const char far *s);

    virtual bool __fastcall ReadPackage(void *lpPkg);
    virtual bool __fastcall WritePackage(const void *lpPkg);
    virtual unsigned long __fastcall ReadPackage(void *lpPkg, unsigned long BufSize);
    virtual unsigned long __fastcall WritePackage(const void *lpPkg, unsigned long nBytes);

    virtual void __fastcall PurgeRead(bool bAbort=false);
    virtual void __fastcall PurgeWrite(bool bAbort=false);

    virtual bool __fastcall SettingsDialog(TComponent* AOwner, bool FActive=false); //return true = OK.
    virtual void __fastcall ResetModem(void); //reset modem to power-on status
    virtual void __fastcall InitModem(void); //init modem using TComm64 properties
    virtual BOOL __fastcall EscapeCommFunction(DWORD dwFunc); //dwFunc: one of the following values: CLRDTR,CLRRTS,SETDTR,SETRTS,SETXOFF,SETXON,SETBREAK,CLRBREAK

    __property HANDLE                Handle        = { read = fGetHandle   };
    __property bool                  UsePackage    = { read = _bUsePackage, write = fSetUsePackage };
    __property TYbCommFrameSettings *FrameSettings = { read = _FrameSettings, write = fSetFrameSettings };

    __property TCommQueue   *InQueue     = { read = fGetInQueue  };
    __property TCommQueue   *OutQueue    = { read = fGetOutQueue };

    __property TVictorComm  *CommPort    = { read = _CommPort    };
    __property TCommPackage *CommPackage = { read = _CommPackage };

    __fastcall TYbCustomCommDevice(TComponent* Owner);
    virtual __fastcall ~TYbCustomCommDevice();

  protected:
    virtual void __fastcall EvCommNotify(TObject *Sender, int NotifyType);
    virtual void __fastcall EvPackageNotify(TObject *Sender, int NotifyType);
    virtual void __fastcall EvAfterOpen(TObject *Sender);
    virtual void __fastcall EvBeforeClose(TObject *Sender);

  private:
    TVictorComm *_CommPort;
    TCommPackage *_CommPackage;
    TYbCommFrameSettings *_FrameSettings;
    bool _bUsePackage, _bSyncEvents;
    HWND          _hNotifyWindow;

    HANDLE __fastcall fGetHandle(void) const;
    void   __fastcall fSetUsePackage(bool bup);
    void   __fastcall fSetFrameSettings(TYbCommFrameSettings *fs);

    TCommQueue *__fastcall fGetInQueue(void) const;
    TCommQueue *__fastcall fGetOutQueue(void) const;

    bool          __fastcall fGetActive(void) const;
    void          __fastcall fSetActive(bool b);
    int           __fastcall fGetPort(void) const;
    void          __fastcall fSetPort(int n);
    UnicodeString __fastcall fGetPortName(void) const;
    void          __fastcall fSetPortName(UnicodeString s);
    UnicodeString __fastcall fGetPortParams(void) const;
    void          __fastcall fSetPortParams(UnicodeString s);
    TBaudRate     __fastcall fGetBaud(void) const;
    void          __fastcall fSetBaud(TBaudRate br);
    DWORD         __fastcall fGetBaudRate(void) const;
    void          __fastcall fSetBaudRate(DWORD dwbr);

    TParity   __fastcall fGetParity(void) const;
    void      __fastcall fSetParity(TParity);
    int       __fastcall fGetByteSize(void) const;
    void      __fastcall fSetByteSize(int);
    TStopBits __fastcall fGetStopBits(void) const;
    void      __fastcall fSetStopBits(TStopBits);

    TFlowControl __fastcall fGetFlCtrl(void) const;
    void __fastcall fSetFlCtrl(TFlowControl);
    int  __fastcall fGetAutAns(void) const;
    void __fastcall fSetAutAns(int);
    TModemStatus __fastcall fGetModemStatus(void) const;
    bool __fastcall fGetDisableWrite(void) const;
    void __fastcall fSetDisableWrite(bool);
    void __fastcall fSetSyncEvents(bool);
    bool __fastcall fGetDataOnly(void) const;
    void __fastcall fSetDataOnly(bool);

    long __fastcall fGetInBufSize(void) const;
    void __fastcall fSetInBufSize(long);
    long __fastcall fGetOutBufSize(void) const;
    void __fastcall fSetOutBufSize(long);

    long __fastcall fGetHwInSize(void) const;
    void __fastcall fSetHwInSize(long);
    long __fastcall fGetHwOutSize(void) const;
    void __fastcall fSetHwOutSize(long);

    long __fastcall fGetQSize(void) const;
    void __fastcall fSetQSize(long);
    long __fastcall fGetPkgSz(void) const;
    void __fastcall fSetPkgSz(long);

    bool __fastcall fGetDTR(void) const;  // Computer status, read/write
    void __fastcall fSetDTR(bool);       // Computer status, read/write
    bool __fastcall fGetRTS(void) const;  // Computer status, read/write
    void __fastcall fSetRTS(bool);       // Computer status, read/write
    bool __fastcall fGetCTS(void) const;  // Modem status, read-only
    bool __fastcall fGetDSR(void) const;  // Modem status, read-only
    bool __fastcall fGetRING(void) const; // Modem status, read-only
    bool __fastcall fGetRLSD(void) const; // Modem status, read-only

    TCommPackageType __fastcall fGetPkgType(void) const;
    void __fastcall fSetPkgType(TCommPackageType);

    long __fastcall fGetPkgInterval(void) const;
    void __fastcall fSetPkgInterval(long);

    void __fastcall _EvNotifyProc(Winapi::Messages::TMessage &Message);
    void __fastcall _EvComm(TObject *Sender, int NotifyType);
    void __fastcall _EvPackage(TObject *Sender, int NotifyType);
    void __fastcall _EvAfterOpen(TObject *Sender);
    void __fastcall _EvBeforeClose(TObject *Sender);

    void __fastcall (__closure *_lpfnCommNotify)(TObject *Sender, int NotifyType);
    void __fastcall (__closure *_lpfnPackageNotify)(TObject *Sender, int NotifyType);
    void __fastcall (__closure *_lpfnAfterOpen) (TObject *Sender);
    void __fastcall (__closure *_lpfnBeforeClose) (TObject *Sender);
};
//---------------------------------------------------------------------------

class PACKAGE TYbCommDevice : public TYbCustomCommDevice
{
  __published:
    __property PortNo;
    __property PortName;
    __property PortParams;

    __property Baud;
    __property BaudRate;
    __property Parity;
    __property ByteSize;
    __property StopBits;

    __property FlowControl;  //for modem flowcontrol
    __property AutoAnswer;   //0: no answer, >0: answer after # rings
    __property SyncEvents = { default = true  }; //synchronize events to main thread
    __property DataOnly   = { default = false };

    __property InBufSize;    //software buffer size
    __property OutBufSize;   //software buffer size
    __property HwInSize;     //hardware/driver buffer size
    __property HwOutSize;    //hardware/driver buffer size

    __property QueueSize;    //default: 16
    __property PackageSize;  //default: 4096
    __property PackageType;  //default: cptFrameTimeout
    __property PackageInterval;//default: 50
    __property UsePackage;   //default: false
    __property FrameSettings;//default: eb,ee,ec

    __property OnPackage;    //EV_RXCHAR|EV_TXEMPTY|EV_CTS|EV_DSR|EV_RLSD|EV_RING|EV_RXFLAG|EV_RX80FULL|EV_ERR
    __property OnCommNotify; //EV_RXCHAR|EV_TXEMPTY|EV_CTS|EV_DSR|EV_RLSD|EV_RING|EV_RXFLAG|EV_RX80FULL|EV_ERR
    __property AfterOpen;
    __property BeforeClose;

  public:
    __fastcall TYbCommDevice(TComponent* Owner);
};
//---------------------------------------------------------------------------
} // namespace VictorComm
} // namespace Vcl
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
