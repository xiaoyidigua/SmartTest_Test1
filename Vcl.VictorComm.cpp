//---------------------------------------------------------------------------
//              TVictorComm.cpp - 串行口通讯程序, 版本 1.5.0.5             //
//                           Built: Jul. 10, 2015                          //
//                    Copyright © 1997-2015, Victor Chen                   //
//                     Homepage: http://www.cppfans.com                    //
//                         Email: victor@cppfans.com                       //
//---------------------------------------------------------------------------
#pragma hdrstop
#include "Vcl.VictorComm.h"
#include <stdio.h>
#include <process.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
namespace Vcl {
namespace VictorComm {
//---------------------------------------------------------------------------
//////////////////////////////// TCommQueue /////////////////////////////////
//---------------------------------------------------------------------------
TCommQueue::TCommQueue(long lSize)
{
  _BufSize   = lSize;
  _Head      = 0;
  _Tail      = 0;
  _Buffer    = NULL;

  try
   {
     fSetBufSize(_BufSize);
   }
  catch(...)
   {
     fSetBufSize(0);
   }
}
//---------------------------------------------------------------------------
TCommQueue::~TCommQueue()
{
  fSetBufSize(0);
}
//---------------------------------------------------------------------------
void TCommQueue::Clear(void)
{
  _Head = _Tail = 0;
}
//---------------------------------------------------------------------------
bool TCommQueue::fIsValid(void)const
{
  return _Buffer != 0;
}
//---------------------------------------------------------------------------
long TCommQueue::fGetCount(void)const
{
  return (_Tail-_Head+_BufSize)%_BufSize;
}
//---------------------------------------------------------------------------
long TCommQueue::fGetRemain(void)const
{
  return (_Head-_Tail+_BufSize-1)%_BufSize;
}
//---------------------------------------------------------------------------
void TCommQueue::fSetBufSize(long n)
 {
   Clear(); //clear buffer before resize

   if(n<=0)
    {
      if(_Buffer)
       {
         free(_Buffer);
         _Buffer = NULL;
       }
    }
   else
    {
      char far *NewBuffer = (char far *)realloc(_Buffer, n);
      if(NewBuffer)
       {
         _Buffer = NewBuffer;
         _BufSize = n;
       }

      if(!NewBuffer)
       {
         throw EVictorCommError(EVictorCommError::COMM_NOMEMORY); //内存不够
       }
    }
 }

//---------------------------------------------------------------------------
long TCommQueue::In(const char far *c)
{
  if((_Buffer) && (c) && (fGetRemain()>0))
   {
     _Buffer[_Tail] = *c;
     _Tail = (_Tail+1)%_BufSize;
     return 1;
   }
  return 0;
}
//---------------------------------------------------------------------------

long TCommQueue::In(const char far *c, long n)
 {
   long BytesToIn = 0;           // 0 1 2 3 4 5 6 7 8 9 <- address
                                 //     ~~~             <- data
   if((_Buffer) && (n>0) && (c)) // ....H...T.......... <- pointers
    {
      long BytesAppend, BytesAhead;

      if((BytesToIn=fGetRemain())>n)
          BytesToIn=n;

      if((BytesAhead=_Tail+BytesToIn-_BufSize)<0)
          BytesAhead=0;

      BytesAppend=BytesToIn-BytesAhead;

      memcpy(_Buffer+_Tail, c            , BytesAppend);
      memcpy(_Buffer      , c+BytesAppend, BytesAhead );

      _Tail = (_Tail + BytesToIn) % _BufSize;
    }

   return BytesToIn;
 }
//---------------------------------------------------------------------------

long TCommQueue::Out(char far *c)
{
  if((_Buffer) && (c) && (fGetCount()>0))
   {
     *c = _Buffer[_Head];
     _Head = (_Head+1)%_BufSize;
     return 1;
   }
  return 0;
}
//---------------------------------------------------------------------------

long TCommQueue::Out(char far *c, long n)
 {
   long BytesToOut = 0;

   if((_Buffer) && (n>0) && (c))
    {
      long BytesTail, BytesAhead;

      if((BytesToOut=fGetCount())>n)
          BytesToOut=n;

      if((BytesAhead=_Head+BytesToOut-_BufSize)<0)
          BytesAhead=0;

      BytesTail=BytesToOut-BytesAhead;

      memcpy(c          , _Buffer+_Head, BytesTail );
      memcpy(c+BytesTail, _Buffer      , BytesAhead);

      _Head = (_Head + BytesToOut) % _BufSize;
    }

   return BytesToOut;
 }
//---------------------------------------------------------------------------
//////////////////////////// TVictorComm Threads ////////////////////////////
//---------------------------------------------------------------------------
void TVictorComm::_ReadThread(void *Param)
{
  TVictorComm *Comm = (TVictorComm *) Param;
  Comm->_ReadThreadRunning = 1;

  COMSTAT ComStat;
  const int RecvBufSize = 2048;
  char RecvBuf[RecvBufSize];
  DWORD dwErrorFlag, dwBytes, BytesToRead, BytesRemain;

  OVERLAPPED os;
  DWORD dwEvtMask, dwModemStatus;

  os.hEvent = CreateEvent(NULL, true, false, NULL);
  if(os.hEvent)
   {
     DWORD dwMask = Comm->DataOnly ? EV_RXCHAR|EV_TXEMPTY|EV_ERR
                                   : EV_RXCHAR|EV_TXEMPTY|EV_CTS|EV_DSR|EV_RING|EV_RLSD|EV_RXFLAG|EV_RX80FULL|EV_ERR;
     if(SetCommMask(Comm->Handle, dwMask))
      {
        HANDLE StatusWaits[2] = {Comm->_hKillRead, os.hEvent};

        while(Comm->_RunReadThread)
         {
           dwEvtMask = 0;
           if(!WaitCommEvent(Comm->Handle, &dwEvtMask, &os))
            {
              if(GetLastError() == ERROR_IO_PENDING)
               {
                 if(WaitForMultipleObjects(2, StatusWaits, false, INFINITE) == WAIT_OBJECT_0)
                   break;
               }
            }

           if(!Comm->_RunReadThread)
             break;

           if(dwEvtMask & EV_RXCHAR)
            {
              ClearCommError(Comm->Handle, &dwErrorFlag, &ComStat);
              BytesRemain = ComStat.cbInQue;

              while(BytesRemain>0)
               {
                 BytesToRead = BytesRemain<RecvBufSize?BytesRemain:RecvBufSize;

                 if(ReadFile(Comm->Handle, RecvBuf, BytesToRead, &dwBytes, &Comm->_ReadOS))
                  {
                    Comm->_InQueue->In(RecvBuf, dwBytes);
                    BytesRemain-=dwBytes;
                  }
                 else
                  {
                    DWORD dwError = GetLastError();
                    if(dwError==ERROR_IO_PENDING)
                     {
                       if(GetOverlappedResult(Comm->Handle, &Comm->_ReadOS, &dwBytes, false))
                        {
                          Comm->_InQueue->In(RecvBuf, dwBytes);
                          BytesRemain-=dwBytes;
                        }
                     }
                  }
               }
            }

           if(dwEvtMask & (EV_CTS|EV_DSR|EV_RING|EV_RLSD))
            {
              if(GetCommModemStatus(Comm->Handle, &dwModemStatus))
               {
                 Comm->_ModemStatus = dwModemStatus;
               }
              if(dwEvtMask & EV_CTS)
               {
                 if((!Comm->FromHandle) && (Comm->_dcb.fOutxCtsFlow)  && (Comm->_ModemStatus & MS_CTS_ON))
                   dwEvtMask |= EV_TXEMPTY;
               }
            }

           if(dwEvtMask & EV_TXEMPTY)
            {
              if((Comm->_ModemStatus & MS_CTS_ON) || (!Comm->_dcb.fOutxCtsFlow) || (Comm->FromHandle))
               {
                 if(Comm->_InQueue->Count)
                   dwEvtMask &=~ EV_TXEMPTY;
                 SetEvent(Comm->_hSyncWrite);
               }
            }

           if(dwEvtMask & EV_ERR)
            {
              ClearCommError(Comm->Handle, &dwErrorFlag, &ComStat);
            }

           if(dwEvtMask)
            {
              Comm->CommNotify(dwEvtMask);
            }
           else // something wrong with this port, but the error was unknown (maybe hardware error or PnP device removed)
            {
              Sleep(1);
            }
         }
      }

     CloseHandle(os.hEvent);
   }

  Comm->_ReadThreadRunning = 0;
}
//---------------------------------------------------------------------------
void TVictorComm::_WriteThread(void *Param)
{
  TVictorComm *Comm = (TVictorComm *) Param;
  Comm->_WriteThreadRunning = 1;

  const int SendBufSize = 512;
  char SendBuf[SendBufSize];
  DWORD BytesSent=0, BytesToSend=0;

  COMSTAT ComStat;
  DWORD dwErrorFlag, dwBytesWr;

  DWORD dwSingled;
  HANDLE MainWaits[2] = {Comm->_hKillWrite, Comm->_hSyncWrite};
  HANDLE OvlWriteWaits[2] = {Comm->_hKillWrite, Comm->_WriteOS.hEvent};

  while(Comm->_RunWriteThread)
   {
     dwSingled = WaitForMultipleObjects(2, MainWaits, false, INFINITE);
     if((!Comm->_RunWriteThread) || (dwSingled==WAIT_OBJECT_0))
       break;
     ResetEvent(Comm->_hSyncWrite);

     if(BytesSent<BytesToSend)
      {
        if(WriteFile(Comm->Handle, SendBuf+BytesSent, BytesToSend-BytesSent, &dwBytesWr, &Comm->_WriteOS))
         {
           BytesSent+=dwBytesWr;
         }
        else if(GetLastError()==ERROR_IO_PENDING)
         {
           dwSingled = WaitForMultipleObjects(2, OvlWriteWaits, false, INFINITE);
           if((!Comm->_RunWriteThread) || (dwSingled==WAIT_OBJECT_0))
             break;
           if(GetOverlappedResult(Comm->Handle, &Comm->_WriteOS, &dwBytesWr, false))
             BytesSent+=dwBytesWr;
         }
        if(BytesSent<BytesToSend)
         {
           SetEvent(Comm->_hSyncWrite);
         }
        Sleep(1);
      }
     else if(Comm->_OutQueue->Count)
      {
        BytesSent = 0;
        BytesToSend = Comm->_OutQueue->Out(SendBuf, SendBufSize);
        SetEvent(Comm->_hSyncWrite);
      }
   }

  Comm->_WriteThreadRunning = 0;
}
//---------------------------------------------------------------------------
//////////////////////////////// TVictorComm ////////////////////////////////
//---------------------------------------------------------------------------
const char TVictorComm::Fmt_InitModem[]  = "ATE0&K%dS0=%d\r"; // Modem 指令使用 ASCII, 非 UNICODE
const char TVictorComm::Str_ResetModem[] = "ATZ\r";           // Modem 指令使用 ASCII, 非 UNICODE
//---------------------------------------------------------------------------
__fastcall TVictorComm::TVictorComm()
{
  _Opened       = false;
  _bFromHandle  = false;
  _DisableWrite = false;
  _bDataOnly    = false;

  _bDTR   = true; //After activate, DTR is true.
  _bRTS   = true; //After activate, RTS is true.

  lpCommNotify  = NULL;
  lpAfterOpen   = NULL;
  lpBeforeClose = NULL;

  _PortNo     = 2;    //"COM2"
  _HwInSize   = 1200;
  _HwOutSize  = 1200;
  _AutoAns    = 0;    //disable auto answer
  _PortName   = TCommSerialPortInfo::PortName(_PortNo);

  //---- DCB settings ----//
  memset(&_dcb, 0, sizeof(DCB)); //Clear DCB
  _dcb.DCBlength = sizeof(DCB);  //DWORD: sizeof(DCB)

  _dcb.BaudRate  = CBR_115200; //DWORD: current baud rate
  _dcb.Parity    = NOPARITY;   //BYTE : 0-4=no,odd,even,mark,space
  _dcb.ByteSize  = 8;          //BYTE : number of bits/byte, 4-8
  _dcb.StopBits  = ONESTOPBIT; //BYTE : 0,1,2 = 1, 1.5, 2

  _dcb.fBinary   = true;  //DWORD: binary mode, no EOF check
  _dcb.fParity   = false; //DWORD: enable parity checking

  _dcb.fOutxCtsFlow      = false;              //DWORD: CTS output flow control
  _dcb.fOutxDsrFlow      = false;              //DWORD: DSR output flow control
  _dcb.fDtrControl       = DTR_CONTROL_ENABLE; //DWORD: DTR flow control type
  _dcb.fDsrSensitivity   = false;              //DWORD: DSR sensitivity

  _dcb.fTXContinueOnXoff = false;              //DWORD: XOFF continues Tx
  _dcb.fOutX             = false;              //DWORD: XON/XOFF out flow control
  _dcb.fInX              = false;              //DWORD: XON/XOFF in flow control
  _dcb.fErrorChar        = false;              //DWORD: enable error replacement
  _dcb.fNull             = false;              //DWORD: enable null stripping
  _dcb.fRtsControl       = RTS_CONTROL_ENABLE; //DWORD: RTS flow control
  _dcb.fAbortOnError     = false;              //WORD : abort reads/writes on error

  _dcb.XonLim            = 2048; //0xffff; //WORD : transmit XON threshold
  _dcb.XoffLim           =  512; //0xffff; //WORD : transmit XOFF threshold

  _dcb.XonChar           = 0x11; //char : Tx and Rx XON character
  _dcb.XoffChar          = 0x13; //char : Tx and Rx XOFF character
  _dcb.ErrorChar         = 0;    //char : error replacement character
  _dcb.EofChar           = 0;    //char : end of input character
  _dcb.EvtChar           = 0;    //char : received event character

  //---- Variables ----//
  _ModemStatus           = 0;   //MS_CTS_ON | MS_DSR_ON | MS_RING_ON | MS_RLSD_ON
  _Handle                = INVALID_HANDLE_VALUE;

  _ReadThreadId          = (uintptr_t)(-1);
  _RunReadThread         =  0;
  _ReadThreadRunning     =  0;

  _WriteThreadId         = (uintptr_t)(-1);
  _RunWriteThread        =  0;
  _WriteThreadRunning    =  0;

  _hKillRead             =  0;
  _hKillWrite            =  0;
  _hSyncWrite            =  0;

  memset(&_ReadOS, 0, sizeof(OVERLAPPED));
  memset(&_WriteOS, 0, sizeof(OVERLAPPED));

  _InQueue  = new TCommQueue;
  _OutQueue = new TCommQueue;
}
//---------------------------------------------------------------------------
__fastcall TVictorComm::~TVictorComm()
{
  try
   {
     Close();
   }
  catch(Exception &e)
   {
     //ignore errors
   }

  delete _OutQueue;
  delete _InQueue;
}
//---------------------------------------------------------------------------
void __fastcall TVictorComm::Open(void)
{
  if(!_Opened)
   {
     EVictorCommError::TErrorCode ecErrCode = EVictorCommError::COMM_NOERROR;

     if((!ecErrCode) && (!_InQueue->Valid))
      {
        ecErrCode = EVictorCommError::COMM_INITRDBUF; //不能初始化读缓存
      }

     if((!ecErrCode) && (!_OutQueue->Valid))
      {
        ecErrCode = EVictorCommError::COMM_INITWRBUF; //不能初始化写缓存
      }

     if((!ecErrCode) && (!_bFromHandle)) //如果已知句柄，不需要打开端口
      {
        UnicodeString sDeviceName = _T("\\\\.\\")+_PortName;
        _Handle=CreateFile(sDeviceName.w_str(), GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, NULL);
        if(_Handle==INVALID_HANDLE_VALUE)
         {
           ecErrCode = EVictorCommError::COMM_OPENPORT; //不能打开端口
         }
      }

     if((!ecErrCode) && (!SetCommMask(_Handle, EV_RXFLAG)))
      {
        ecErrCode = EVictorCommError::COMM_SETMASK; //不能设置端口事件MASK
      }

     if((!ecErrCode) && (!_bFromHandle)) //如果已知句柄，不需要设置缓存
      {
        if(!SetupComm(_Handle, _HwInSize, _HwOutSize))
         {
           ecErrCode = EVictorCommError::COMM_BUFSIZE; //不能设置端口缓存
         }
      }

     if(!ecErrCode)
      {
        if(!_bFromHandle)
         {
           //设置预先设定的 DTR 和 RTS 值
           if(_dcb.fDtrControl != DTR_CONTROL_HANDSHAKE)
            {
              _dcb.fDtrControl = _bDTR?DTR_CONTROL_ENABLE:DTR_CONTROL_DISABLE;
            }
           if((_dcb.fRtsControl != RTS_CONTROL_HANDSHAKE) && (_dcb.fRtsControl != RTS_CONTROL_TOGGLE))
            {
              _dcb.fRtsControl = _bRTS?RTS_CONTROL_ENABLE:RTS_CONTROL_DISABLE;
            }

           DCB dcb = _dcb; //设定串口参数
           if(!SetCommState(_Handle, &dcb))
            {
              ecErrCode = EVictorCommError::COMM_SETSTATE; //不能设置端口参数
            }
         }
        else //如果已知句柄，不需要设置参数
         {
           DCB dcb;
           if(GetCommState(_Handle, &dcb))
            {
              dcb.fAbortOnError = false;
              if(!SetCommState(_Handle, &dcb))
               {
                 ecErrCode = EVictorCommError::COMM_SETSTATE; //不能设置端口参数
               }
            }
           else
            {
              ecErrCode = EVictorCommError::COMM_GETSTATE; //不能得到端口参数
            }
         }
      }

     if(!ecErrCode)
      {
        memset(&_ReadOS, 0, sizeof(OVERLAPPED));
        _ReadOS.hEvent = CreateEvent(NULL,true,false,NULL);
        _hKillRead = CreateEvent(NULL,true,false,NULL);

        if((!_hKillRead) || (!_ReadOS.hEvent))
         {
           ecErrCode = EVictorCommError::COMM_CRRDEVENT; //不能创建端口异步读事件
         }
      }

     if(!ecErrCode)
      {
        memset(&_WriteOS, 0, sizeof(OVERLAPPED));
        _WriteOS.hEvent = CreateEvent(NULL, true, false, NULL);
        _hSyncWrite = CreateEvent(NULL, true, false, NULL);
        _hKillWrite = CreateEvent(NULL, true, false, NULL);

        if((!_hKillWrite) || (!_hSyncWrite) || (!_WriteOS.hEvent))
         {
           ecErrCode = EVictorCommError::COMM_CRWREVENT; //不能创建端口异步写事件
         }
      }

     if(!ecErrCode)
      {
        _RunReadThread = 1;
        if((_ReadThreadId=_beginthread(_ReadThread, 4096, this)) == (uintptr_t)(-1))
         {
           _RunReadThread = 0;
           ecErrCode = EVictorCommError::COMM_CRRDTHREAD; //不能创建端口读线程
         }
      }

     if(!ecErrCode)
      {
        _RunWriteThread = 1;
        if((_WriteThreadId=_beginthread(_WriteThread, 4096, this)) == (uintptr_t)(-1))
         {
           _RunWriteThread = 0;
           ecErrCode = EVictorCommError::COMM_CRWRTHREAD; //不能创建端口写线程
         }
      }

     if(!ecErrCode)
      {
        SetThreadPriority((HANDLE)_ReadThreadId,THREAD_PRIORITY_HIGHEST);
        //SetThreadPriority((HANDLE)_WriteThreadId,THREAD_PRIORITY_HIGHEST);

        _Opened = true;
        _DisableWrite = false;

        DWORD dwMs; //MS_CTS_ON | MS_DSR_ON | MS_RING_ON | MS_RLSD_ON
        if(GetCommModemStatus(_Handle, &dwMs)) //Get modem status after port opened
          _ModemStatus = dwMs;

        _bDTR = _dcb.fDtrControl != DTR_CONTROL_DISABLE;
        _bRTS = _dcb.fRtsControl != RTS_CONTROL_DISABLE;

        CommNotify(EV_CTS|EV_DSR|EV_RLSD);
        CommAfterOpen();
      }
     else
      {
        Close();
        throw EVictorCommError(ecErrCode,PortName);
      }
   }
}
//---------------------------------------------------------------------------
void __fastcall TVictorComm::Close(void)
{
  EVictorCommError::TErrorCode ecErrCode = EVictorCommError::COMM_NOERROR;
  int i;

  if(_Opened)
   {
     CommBeforeClose();
   }

  _RunReadThread = 0;
  _RunWriteThread = 0;

  if(_Handle!=INVALID_HANDLE_VALUE)
   {
     SetCommMask(_Handle, EV_RXFLAG); //terminate the WaitCommEvent() func.
   }
  if(_hKillRead )SetEvent(_hKillRead );
  if(_hKillWrite)SetEvent(_hKillWrite);

  //-- read & write thread --
  for(i=0; (_ReadThreadRunning||_WriteThreadRunning) && (i<100); i++)
   {
     Sleep(5);
   }

  if(_ReadThreadRunning)
   {
     ecErrCode = EVictorCommError::COMM_CLOSERDT; //不能关闭端口读线程
   }
  if(_WriteThreadRunning)
   {
     ecErrCode = EVictorCommError::COMM_CLOSEWRT; //不能关闭端口写线程
   }

  _ReadThreadId  = (uintptr_t)(-1);
  _WriteThreadId = (uintptr_t)(-1);

  //-- read event
  if(_ReadOS.hEvent)
   {
     CloseHandle(_ReadOS.hEvent);
     memset(&_ReadOS, 0, sizeof(OVERLAPPED));
   }

  if(_hKillRead)
   {
     CloseHandle(_hKillRead);
     _hKillRead = NULL;
   }

  //-- write sync --
  if(_hSyncWrite)
   {
     CloseHandle(_hSyncWrite);
     _hSyncWrite = NULL;
   }

  if(_WriteOS.hEvent)
   {
     CloseHandle(_WriteOS.hEvent);
     memset(&_WriteOS, 0, sizeof(OVERLAPPED));
   }

  if(_hKillWrite)
   {
     CloseHandle(_hKillWrite);
     _hKillWrite = NULL;
   }

  //-- close handle --
  if((_Handle!=INVALID_HANDLE_VALUE) && (!_bFromHandle)) //如果已知句柄，不需要关闭端口
   {
     CloseHandle(_Handle);
     _Handle=INVALID_HANDLE_VALUE;
   }

  //-- Restore status before opened
  _bDTR = _dcb.fDtrControl != DTR_CONTROL_DISABLE;
  _bRTS = _dcb.fRtsControl != RTS_CONTROL_DISABLE;

  if(_Opened)
   {
     _Opened = false;
     _ModemStatus = 0; //All modem status OFF after port closed //MS_CTS_ON | MS_DSR_ON | MS_RING_ON | MS_RLSD_ON
     CommNotify(EV_CTS|EV_DSR|EV_RLSD);
   }

  //-- error msg. --
  if(ecErrCode)
   {
     throw EVictorCommError(ecErrCode,PortName);
   }
}
//---------------------------------------------------------------------------
void __fastcall TVictorComm::fSetPort(int COMn)
{
  Close();
  _PortNo = COMn;
  if(_PortNo>0)
    _PortName = TCommSerialPortInfo::PortName(_PortNo);
  _bFromHandle = false;
}
//---------------------------------------------------------------------------
void __fastcall TVictorComm::fSetPortName(UnicodeString s)
{
  if(!s.IsEmpty())
   {
     int COMn = TCommSerialPortInfo::PortNo(s);
     Close();
     _PortNo = COMn;
     _PortName = _PortNo>0 ? TCommSerialPortInfo::PortName(_PortNo) : s;
     _bFromHandle = false;
   }
}
//---------------------------------------------------------------------------
void __fastcall TVictorComm::fSetHandle(HANDLE hFile)
{
  Close();
  _PortNo = 0;
  _PortName = _T("");
  _Handle = hFile;
  _bFromHandle = true;
}
//---------------------------------------------------------------------------
TVictorComm::TFlowControl __fastcall TVictorComm::fGetFCtrl(void)
{
  if(_dcb.fOutxCtsFlow && _dcb.fOutX)
   {
     return fcRtsCtsXonXoff; //6
   }

  if(_dcb.fOutxCtsFlow)
   {
     return fcRtsCts; //3
   }

  if(_dcb.fOutX)
   {
     return fcXonXoff; //4
   }

  //fcTranXonXoff; //5 (not supported)
  return fcNone; //0
}
//---------------------------------------------------------------------------
void __fastcall TVictorComm::fSetFCtrl(TFlowControl fctrl)
{
  Close();
  switch(fctrl)
   {
     case fcRtsCts:
             _dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
             _dcb.fOutxCtsFlow = true;

             _dcb.fDtrControl = DTR_CONTROL_ENABLE;
             _dcb.fOutxDsrFlow = false;

             _dcb.fInX = false;
             _dcb.fOutX = false;
             break;

     case fcXonXoff:
     case fcTranXonXoff:
             _dcb.fRtsControl = RTS_CONTROL_ENABLE;
             _dcb.fOutxCtsFlow = false;

             _dcb.fDtrControl = DTR_CONTROL_ENABLE;
             _dcb.fOutxDsrFlow = false;

             _dcb.fInX = true;
             _dcb.fOutX = true;
             break;

     case fcRtsCtsXonXoff:
             _dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
             _dcb.fOutxCtsFlow = true;

             _dcb.fDtrControl = DTR_CONTROL_ENABLE;
             _dcb.fOutxDsrFlow = false;

             _dcb.fInX = true;
             _dcb.fOutX = true;
             break;

     default: //fcNone:
             _dcb.fRtsControl = RTS_CONTROL_ENABLE;
             _dcb.fOutxCtsFlow = false;

             _dcb.fDtrControl = DTR_CONTROL_ENABLE;
             _dcb.fOutxDsrFlow = false;

             _dcb.fInX = false;
             _dcb.fOutX = false;
             break;
   }
}
//---------------------------------------------------------------------------
bool __fastcall TVictorComm::fGetDTR(void)const //Computer status, read/write
{
  if(!Active)
    return false;
  return _bDTR;
}
//---------------------------------------------------------------------------
bool __fastcall TVictorComm::fGetRTS(void)const //Computer status, read/write
{
  if(!Active)
    return false;
  return _bRTS;
}
//---------------------------------------------------------------------------
void __fastcall TVictorComm::fSetDTR(bool b) //Computer status, read/write
{
  if(Active)
    EscapeCommFunction(b?SETDTR:CLRDTR);
  else
    _bDTR = b; //change DTR status after open
}
//---------------------------------------------------------------------------
void __fastcall TVictorComm::fSetRTS(bool b) //Computer status, read/write
{
  if(Active)
    EscapeCommFunction(b?SETRTS:CLRRTS);
  else
    _bRTS = b; //change RTS status after open
}
//---------------------------------------------------------------------------
long __fastcall TVictorComm::Read(void far *s, long n)
{
  return _InQueue->Out((char far *)s, n);
}
//---------------------------------------------------------------------------
long __fastcall TVictorComm::Write(const void far *s, long n)
{
  if(_DisableWrite)
    return 0;

  long Retv = _OutQueue->In((const char far *)s, n);
  SetEvent(_hSyncWrite);
  return Retv;
}
//---------------------------------------------------------------------------
long __fastcall TVictorComm::Command(const char far *s)
{
  long Retv = _OutQueue->In(s,strlen(s));
  SetEvent(_hSyncWrite);
  return Retv;
}
//---------------------------------------------------------------------------
void __fastcall TVictorComm::PurgeRead(bool bAbort)
{
  if(_Opened)
   {
     DWORD WhatToPurge = PURGE_RXCLEAR;
     if(bAbort)WhatToPurge|=PURGE_RXABORT;
     PurgeComm(_Handle, WhatToPurge);
     _InQueue->Clear();
   }
}
//---------------------------------------------------------------------------
void __fastcall TVictorComm::PurgeWrite(bool bAbort)
{
  if(_Opened)
   {
     DWORD WhatToPurge = PURGE_TXCLEAR;
     if(bAbort)WhatToPurge|=PURGE_TXABORT;
     PurgeComm(_Handle, WhatToPurge);
     _OutQueue->Clear();
   }
}
//---------------------------------------------------------------------------
void __fastcall TVictorComm::ResetModem(void) //reset modem to factory settings
{
  Command(Str_ResetModem);
}
//---------------------------------------------------------------------------
void __fastcall TVictorComm::InitModem(void) //init modem using TVictorComm properties
{
  char szCmd[64]; // MODEM 命令为 ASCII (char) 格式的
  sprintf(szCmd, Fmt_InitModem, (int)FlowControl, (int)AutoAnswer);
  Command(szCmd);
}
//---------------------------------------------------------------------------
BOOL __fastcall TVictorComm::EscapeCommFunction(DWORD dwFunc) //dwFunc: one of the following values: CLRDTR,CLRRTS,SETDTR,SETRTS,SETXOFF,SETXON,SETBREAK,CLRBREAK
{
  if(Active)
   {
     if(::EscapeCommFunction(Handle,dwFunc))
      {
        switch(dwFunc)
         {
           case SETRTS: _bRTS=true ; break;
           case CLRRTS: _bRTS=false; break;
           case SETDTR: _bDTR=true ; break;
           case CLRDTR: _bDTR=false; break;
         }
        return true;
      }
   }
  return false;
}
//---------------------------------------------------------------------------
void __fastcall TVictorComm::CommNotify(int NotifyType)
{
  if(lpCommNotify)
   {
     try
      {
        lpCommNotify(this, NotifyType);
      }
     catch(...)
      {
        //do nothing (errors occurred on user's event)
      }
   }
}
//---------------------------------------------------------------------------
void __fastcall TVictorComm::CommAfterOpen(void)
{
  if(lpAfterOpen)
   {
     try
      {
        lpAfterOpen(this);
      }
     catch(...)
      {
        //do nothing (errors occurred on user's event)
      }
   }
}
//---------------------------------------------------------------------------

void __fastcall TVictorComm::CommBeforeClose(void)
{
  if(lpBeforeClose)
   {
     try
      {
        lpBeforeClose(this);
      }
     catch(...)
      {
        //do nothing (errors occurred on user's event)
      }
   }
}
//---------------------------------------------------------------------------
void __fastcall TVictorComm::fSetActive(bool bActive) { if(bActive) Open(); else Close(); }
void __fastcall TVictorComm::fSetDataOnly(bool bDO) { Close(); _bDataOnly=bDO; }

DWORD __fastcall TVictorComm::fGetBaud    (void)const { return _dcb.BaudRate; }
BYTE  __fastcall TVictorComm::fGetParity  (void)const { return _dcb.Parity  ; }
BYTE  __fastcall TVictorComm::fGetByteSize(void)const { return _dcb.ByteSize; }
BYTE  __fastcall TVictorComm::fGetStopBits(void)const { return _dcb.StopBits; }

void __fastcall TVictorComm::fSetBaud    (DWORD NewBaud  ) { Close(); _dcb.BaudRate = NewBaud  ; }
void __fastcall TVictorComm::fSetParity  (BYTE  NewParity) { Close(); _dcb.Parity   = NewParity; }
void __fastcall TVictorComm::fSetByteSize(BYTE  NewBSize ) { Close(); _dcb.ByteSize = NewBSize ; }
void __fastcall TVictorComm::fSetStopBits(BYTE  NewSBits ) { Close(); _dcb.StopBits = NewSBits ; }

long __fastcall TVictorComm::fGetInBufSize (void)const { return _InQueue ->QSize; }
long __fastcall TVictorComm::fGetOutBufSize(void)const { return _OutQueue->QSize; }
void __fastcall TVictorComm::fSetInBufSize (long n)    { _InQueue ->QSize = n;    }
void __fastcall TVictorComm::fSetOutBufSize(long n)    { _OutQueue->QSize = n;    }

void __fastcall TVictorComm::fSetHwBufSize(long HwIn, long HwOut) { Close(); _HwInSize=HwIn; _HwOutSize=HwOut; }
void __fastcall TVictorComm::fSetHwInSize (long NewSize) { fSetHwBufSize(NewSize, _HwOutSize); }
void __fastcall TVictorComm::fSetHwOutSize(long NewSize) { fSetHwBufSize(_HwInSize,  NewSize); }

void __fastcall TVictorComm::fSetAAns(unsigned char aans) { _AutoAns = aans; }

bool __fastcall TVictorComm::fGetCTS (void)const { return (_ModemStatus & MS_CTS_ON ) != 0; } //Modem status, read-only
bool __fastcall TVictorComm::fGetDSR (void)const { return (_ModemStatus & MS_DSR_ON ) != 0; } //Modem status, read-only
bool __fastcall TVictorComm::fGetRING(void)const { return (_ModemStatus & MS_RING_ON) != 0; } //Modem status, read-only
bool __fastcall TVictorComm::fGetRLSD(void)const { return (_ModemStatus & MS_RLSD_ON) != 0; } //Modem status, read-only
//---------------------------------------------------------------------------
//////////////////////////// TVictorCommLanguage ////////////////////////////
//---------------------------------------------------------------------------
TVictorCommLanguage::TVictorCommLanguage()
{
  LCID id = GetThreadLocale();
  if(!id){ id = lcidEnu; }
  fSetLcId(id);
}
//---------------------------------------------------------------------------
void TVictorCommLanguage::fSetLcId(LCID id)
{
  _lcId = id;
}
//---------------------------------------------------------------------------
//////////////////////////// EVictorCommError ///////////////////////////////
//---------------------------------------------------------------------------
__fastcall EVictorCommError::EVictorCommError(TErrorCode ecErrCode)
                        :Exception(MessageStrings(_T("")).ErrMsg[ecErrCode])
{
  _ErrorCode = ecErrCode;
}
//---------------------------------------------------------------------------
__fastcall EVictorCommError::EVictorCommError(TErrorCode ecErrCode, UnicodeString sPortName)
                        :Exception(MessageStrings(sPortName).ErrMsg[ecErrCode])
{
  _ErrorCode = ecErrCode;
  _PortName = sPortName;
}
//---------------------------------------------------------------------------
__fastcall EVictorCommError::MessageStrings::MessageStrings(UnicodeString sPortName)
{
  _PortName = sPortName;
  switch(_CommLanguage.lcId)
   {
     case TVictorCommLanguage::lcidChs:
          Err_Messages = Err_Messages_Chs;
          Err_PortName = Err_PortName_Chs;
          Err_Unknown  = Err_Unknown_Chs ;
          break;

     case TVictorCommLanguage::lcidCht:
          Err_Messages = Err_Messages_Cht;
          Err_PortName = Err_PortName_Cht;
          Err_Unknown  = Err_Unknown_Cht ;
          break;

     default:
          Err_Messages = Err_Messages_Enu;
          Err_PortName = Err_PortName_Enu;
          Err_Unknown  = Err_Unknown_Enu ;
          break;
   }
}
//---------------------------------------------------------------------------
UnicodeString __fastcall EVictorCommError::MessageStrings::fGetErrMsg(TErrorCode ecCode)
{
  UnicodeString sMsg;
  if(_PortName.Length()>0)
   {
     sMsg.sprintf(Err_PortName, _PortName.w_str());
   }
  for(ERRMSG *em=Err_Messages; em->Msg; em++)
   {
     if(em->Code==ecCode)
       return sMsg+em->Msg;
   }
  return sMsg+Err_Unknown;
}
//---------------------------------------------------------------------------
EVictorCommError::MessageStrings::ERRMSG EVictorCommError::MessageStrings::Err_Messages_Chs[] =
{
  {COMM_NOMEMORY  , L"内存不够"                  },
  {COMM_INITRDBUF , L"不能初始化读缓存"          },
  {COMM_INITWRBUF , L"不能初始化写缓存"          },
  {COMM_OPENPORT  , L"不能打开串行口"            },
  {COMM_SETMASK   , L"不能设置串行口事件MASK"    },
  {COMM_BUFSIZE   , L"不能设置串行口缓存"        },
  {COMM_GETSTATE  , L"不能得到串行口参数"        },
  {COMM_SETSTATE  , L"不能设置串行口参数"        },
  {COMM_CRRDEVENT , L"不能创建串行口异步读事件"  },
  {COMM_CRWREVENT , L"不能创建串行口异步写事件"  },
  {COMM_CRRDTHREAD, L"不能创建串行口读线程"      },
  {COMM_CRWRTHREAD, L"不能创建串行口写线程"      },
  {COMM_CLOSERDT  , L"不能关闭串行口读线程"      },
  {COMM_CLOSEWRT  , L"不能关闭串行口写线程"      },
  {COMM_NOERROR   , 0}, //没有错误
};
//---------------------------------------------------------------------------
EVictorCommError::MessageStrings::ERRMSG EVictorCommError::MessageStrings::Err_Messages_Cht[] =
{
  {COMM_NOMEMORY  , L"記憶體不夠"                },
  {COMM_INITRDBUF , L"不能初始化讀緩存"          },
  {COMM_INITWRBUF , L"不能初始化寫緩存"          },
  {COMM_OPENPORT  , L"不能打開序列埠"            },
  {COMM_SETMASK   , L"不能設置序列埠事件MASK"    },
  {COMM_BUFSIZE   , L"不能設置序列埠緩存"        },
  {COMM_GETSTATE  , L"不能得到序列埠參數"        },
  {COMM_SETSTATE  , L"不能設置序列埠參數"        },
  {COMM_CRRDEVENT , L"不能創建序列埠非同步讀事件"},
  {COMM_CRWREVENT , L"不能創建序列埠非同步寫事件"},
  {COMM_CRRDTHREAD, L"不能創建序列埠讀執行緒"    },
  {COMM_CRWRTHREAD, L"不能創建序列埠寫執行緒"    },
  {COMM_CLOSERDT  , L"不能關閉序列埠讀執行緒"    },
  {COMM_CLOSEWRT  , L"不能關閉序列埠寫執行緒"    },
  {COMM_NOERROR   , 0}, //沒有錯誤
};
//---------------------------------------------------------------------------
EVictorCommError::MessageStrings::ERRMSG EVictorCommError::MessageStrings::Err_Messages_Enu[] =
{
  {COMM_NOMEMORY  , L"Out of memory"                                  },
  {COMM_INITRDBUF , L"Cannot initialize read buffer"                  },
  {COMM_INITWRBUF , L"Cannot initialize write buffer"                 },
  {COMM_OPENPORT  , L"Cannot open COM port"                           },
  {COMM_SETMASK   , L"Cannot set comm-mask"                           },
  {COMM_BUFSIZE   , L"Cannot set COM port buffer size"                },
  {COMM_GETSTATE  , L"Cannot read COM port parameters"                },
  {COMM_SETSTATE  , L"Cannot set COM port parameters"                 },
  {COMM_CRRDEVENT , L"Cannot create COM port asynchronize read event" },
  {COMM_CRWREVENT , L"Cannot create COM port asynchronize write event"},
  {COMM_CRRDTHREAD, L"Cannot create COM port read thread"             },
  {COMM_CRWRTHREAD, L"Cannot create COM port write thread"            },
  {COMM_CLOSERDT  , L"Cannot close COM port read thread"              },
  {COMM_CLOSEWRT  , L"Cannot close COM port write thread"             },
  {COMM_NOERROR   , 0}, //沒有錯誤
};
//---------------------------------------------------------------------------
wchar_t EVictorCommError::MessageStrings::Err_PortName_Chs[] = L"串行口:“%s”- ";
wchar_t EVictorCommError::MessageStrings::Err_PortName_Cht[] = L"序列埠:『%s』- ";
wchar_t EVictorCommError::MessageStrings::Err_PortName_Enu[] = L"Serial port: \"%s\" - ";
//---------------------------------------------------------------------------
wchar_t EVictorCommError::MessageStrings::Err_Unknown_Chs[] = L"未知错误";
wchar_t EVictorCommError::MessageStrings::Err_Unknown_Cht[] = L"未知錯誤";
wchar_t EVictorCommError::MessageStrings::Err_Unknown_Enu[] = L"Unknown error";
//---------------------------------------------------------------------------
} // namespace VictorComm
} // namespace Vcl
//---------------------------------------------------------------------------
