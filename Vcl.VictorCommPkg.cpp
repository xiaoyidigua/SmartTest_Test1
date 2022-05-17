/***************************************************************************\
*                                                                           *
*           TVictorCommPkg.cpp - 串行口通讯数据包协议, 版本 1.5.0.5         *
*                             Built: Jul. 10, 2015                          *
*                     Copyright © 1997-2015, Victor Chen                    *
*                      Homepage: http://www.cppfans.com                     *
*                          Email: victor@cppfans.com                        *
*                                                                           *
\***************************************************************************/

#include "Vcl.VictorCommPkg.h"
#include <process.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
namespace Vcl {
namespace VictorComm {
//---------------------------------------------------------------------------
__fastcall TCommPackageQueue::TCommPackageQueue(unsigned long QSz, unsigned long PkgSz)
{
  _QBuffer = NULL;
  _Head = _Tail = 0;
  SetQueue(QSz, PkgSz);
}
//---------------------------------------------------------------------------
__fastcall TCommPackageQueue::~TCommPackageQueue()
{
  fRemoveQueue();
}
//---------------------------------------------------------------------------
void __fastcall TCommPackageQueue::Clear(void)
{
  _Head = _Tail = 0;
}
//---------------------------------------------------------------------------
unsigned long __fastcall TCommPackageQueue::fGetCount(void)const
{
  return (_Tail-_Head+_QSize)%_QSize;
}
//---------------------------------------------------------------------------
unsigned long __fastcall TCommPackageQueue::fGetRemain(void)const
{
  return (_Head-_Tail+_QSize-1)%_QSize;
}
//---------------------------------------------------------------------------
bool __fastcall TCommPackageQueue::fIsValid(void)const
{
  return _QBuffer != NULL;
}
//---------------------------------------------------------------------------
void __fastcall TCommPackageQueue::fSetQSize(unsigned long qs)
{
  SetQueue(qs, _PkgSize);
}
//---------------------------------------------------------------------------
void __fastcall TCommPackageQueue::fSetPkgSz(unsigned long ps)
{
  SetQueue(_QSize,   ps);
}
//---------------------------------------------------------------------------
void __fastcall TCommPackageQueue::fRemoveQueue(void)
{
  if(_QBuffer)
   {
     delete[] _QBuffer;
     _QBuffer = NULL;
   }
  _Head = _Tail = 0;
}
//---------------------------------------------------------------------------

void __fastcall TCommPackageQueue::SetQueue(long QSz, long PkgSz)
{
  fRemoveQueue();
  _QSize   = QSz>2 ? QSz:2;
  _PkgSize = PkgSz>32 ? PkgSz:32;
  _Head    = _Tail = 0;
  _QBuffer = new char[_QSize*_PkgSize];
}
//---------------------------------------------------------------------------

long __fastcall TCommPackageQueue::In(const void *lpPkg)
{
  if((_QBuffer) && (fGetRemain()>0))
   {
     memcpy(_QBuffer+_PkgSize*_Tail, lpPkg, _PkgSize); //_Buffer[_Tail] = *lpPkg;
     _Tail = (_Tail+1)%_QSize;
     return 1;
   }
  return 0;
}
//---------------------------------------------------------------------------

long __fastcall TCommPackageQueue::Out(void *lpPkg)
{
  if((_QBuffer) && (fGetCount()>0))
   {
     memcpy(lpPkg, _QBuffer+_PkgSize*_Head, _PkgSize); //*lpPkg = _Buffer[_Head];
     _Head = (_Head+1)%_QSize;
     return 1;
   }
  return 0;
}
//---------------------------------------------------------------------------

long __fastcall TCommPackageQueue::Read(void *lpPkg, unsigned long iPkgNo)
{
  if((_QBuffer) && (fGetCount()>0) && (iPkgNo<fGetCount()))
   {
     memcpy(lpPkg, _QBuffer+_PkgSize*((_Head+iPkgNo)%_QSize), _PkgSize);
     return 1;
   }
  return 0;
}
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
__fastcall TCommPackage::TCommPackage(TVictorComm *lpComm)
{
  _bActive = false;
  _bDoubleFrameMark = false;

  _FRAMEHEAD = DefaultFrameHead;
  _FRAMETAIL = DefaultFrameTail;
  _FRAMECTRL = DefaultFrameCtrl;

  _bFrameHeadDetected = false;
  _bControlDetected   = false;
  _BufferBytesWrite   = 0;

  _BufferRead  = NULL;
  _BufferWrite = NULL;
  _BufferTemp  = NULL;

  _QueueSize       = 16;
  _PackageDataSize = 4096;
  _PackageType     = cptFrameTimeout;

  _AllocBuffer();
  _BufferRead->Length = 0;
  _BufferWrite->Length = 0;
  _bWritingPackage = false;

  _QueueRead = new TCommPackageQueue(_QueueSize,_PackageDataSize+4); //包括字节数4字节
  _QueueWrite = new TCommPackageQueue(_QueueSize,_PackageDataSize+4); //包括字节数4字节

  _dwTimerInterval     = 50;    //50 ms
  _iSkipTimerEvents    = 0;     //no skip
  _bTimerThreadRunning = false; //not running
  _bRunTimerThread     = true;  //run enabled

  _hTimerEvent = CreateEvent(NULL, false, false, NULL); //Create Auto-Reset Event
  _hTimerThreadId = _beginthread(_fnTimerThread, 4096, this);

  _CommPort = lpComm;
  lpOldCommNotify = _CommPort->OnCommNotify;
  lpUserCommNotify = NULL; //no default
  _CommPort->OnCommNotify = fNewCommNotify;
}
//---------------------------------------------------------------------------

__fastcall TCommPackage::~TCommPackage()
{
  _CommPort->OnCommNotify = lpOldCommNotify;

  _bRunTimerThread = false;
  SetEvent(_hTimerEvent);
  for(int i=0; (i<20)&&(_bTimerThreadRunning); i++)
    Sleep(5); //wait for timer thread terminate
  CloseHandle(_hTimerEvent);

  delete _QueueWrite;
  delete _QueueRead;

  _RemoveBuffer();
}
//---------------------------------------------------------------------------
void __fastcall TCommPackage::_AllocBuffer(void)
{
  char *TmpBuf;
  _RemoveBuffer();

  TmpBuf = new char[_PackageDataSize+4]; //包括字节数4字节
  _BufferRead = (TCommPackageHead*)TmpBuf;
  _BufferRead->Length = 0;

  TmpBuf = new char[_PackageDataSize*2+4+4]; //HD HD DATA*2 TL TL 包括首尾4字节，字节数4字节
  _BufferWrite = (TCommPackageHead*)TmpBuf;
  _BufferWrite->Length = 0;

  TmpBuf = new char[_PackageDataSize+4]; //包括字节数4字节
  _BufferTemp = (TCommPackageHead*)TmpBuf;
  _BufferTemp->Length = 0;
}
//---------------------------------------------------------------------------
void __fastcall TCommPackage::_RemoveBuffer(void)
{
  char *TmpBuf;
  _BufferBytesWrite = 0;

  TmpBuf = (char*)_BufferTemp;
  if(TmpBuf)
   {
     delete[]TmpBuf;
     _BufferTemp = NULL;
   }

  TmpBuf = (char*)_BufferWrite;
  if(TmpBuf)
   {
     delete[]TmpBuf;
     _BufferWrite = NULL;
   }

  TmpBuf = (char*)_BufferRead;
  if(TmpBuf)
   {
     delete[]TmpBuf;
     _BufferRead = NULL;
   }
}
//---------------------------------------------------------------------------
void TCommPackage::fSetQSize(unsigned long qs)
{
  bool bOldActive = Active;
  if(bOldActive)Active = false;

  _QueueSize = qs;
  _QueueRead->QSize = _QueueSize;
  _QueueWrite->QSize = _QueueSize;

  if(bOldActive)Active = true;
}
//---------------------------------------------------------------------------
void TCommPackage::fSetPkgSz(unsigned long ps)
{
  bool bOldActive = Active;
  if(bOldActive)Active = false;

  _PackageDataSize = ps;
  _QueueRead->PkgSize = _PackageDataSize+4; //包括字节数4字节
  _QueueWrite->PkgSize = _PackageDataSize+4; //包括字节数4字节
  _AllocBuffer(); //调整数据包大小

  if(bOldActive)Active = true;
}
//---------------------------------------------------------------------------
void TCommPackage::fSetPkgType(TCommPackageType pt)
{
  bool bOldActive = Active;
  if(bOldActive)Active = false;

  _PackageType = pt;

  if(bOldActive)Active = true;
}
//---------------------------------------------------------------------------
void TCommPackage::fSetPkgInterval(DWORD i)
{
  _dwTimerInterval = i;
  SetEvent(_hTimerEvent);
}
//---------------------------------------------------------------------------
void TCommPackage::fSetDblFrameMark(bool b)
{
  _bDoubleFrameMark = b;
}
//---------------------------------------------------------------------------
void __fastcall TCommPackage::fSetActive(bool b)
{
  _bActive = b;

  _bFrameHeadDetected = false;
  _bControlDetected   = false;
  _BufferBytesWrite   = 0;

  _BufferRead->Length = 0;
  _BufferWrite->Length = 0;

  _QueueRead->Clear();
  _QueueWrite->Clear();

  SetEvent(_hTimerEvent);
}
//---------------------------------------------------------------------------
void __fastcall TCommPackage::fNewCommNotify(TObject *Sender, int NotifyType)
{
  if(_bActive)
   {
     int UserNotify = NotifyType&~(EV_RXCHAR|EV_TXEMPTY);

     if((NotifyType & EV_RXCHAR) && (_PackageType != cptFrameTimeout))
      {
        _ReadHeadTailPackage();

        if(_QueueRead->Count)
         {
           UserNotify |= EV_RXCHAR;
         }
      }

     if(NotifyType & EV_TXEMPTY)
      {
        _WritePackage();

        if(!_QueueWrite->Count)
         {
           UserNotify |= EV_TXEMPTY;
         }
      }

     if(UserNotify)
      {
        CallNotify(UserNotify);
      }
   }

  if(lpOldCommNotify)
   {
     lpOldCommNotify(Sender, NotifyType);
   }
}
//---------------------------------------------------------------------------
void __fastcall TCommPackage::CallNotify(int NotifyType)
{
  if(lpUserCommNotify)
   {
     lpUserCommNotify(this, NotifyType);
   }
}
//---------------------------------------------------------------------------
void _USERENTRY TCommPackage::_fnTimerThread(void *pm)
{
  TCommPackage *lpPkg = (TCommPackage *)pm;
  if(lpPkg)lpPkg->_TimerThread();
}
//---------------------------------------------------------------------------
unsigned long __fastcall TCommPackage::ReadPackage(void *lpPkg, unsigned long BufSize)
{
  if(_bActive)
   {
     if(_PackageType==cptCompatiblePkg)
      {
        return _QueueRead->Out(lpPkg);
      }
     else
      {
        unsigned long n = 0;
        char *Buf = new char[_QueueRead->PkgSize];
        try
         {
           if(BufSize && lpPkg)
            {
              if(_QueueRead->Out(Buf))
               {
                 n = ((TCommPackageHead*)Buf)->Length;
                 if(n>BufSize)n=BufSize;
                 memcpy(lpPkg, ((TCommPackageHead*)Buf)->Data, n);
               }
            }
           else //只返回数据包的数据字节数
            {
              if(_QueueRead->Read(Buf,0))
                n = ((TCommPackageHead*)Buf)->Length;
            }
         }
        __finally
         {
           delete[] Buf;
         }
        return n;
      }
   }
  return 0;
}
//---------------------------------------------------------------------------
unsigned long __fastcall TCommPackage::WritePackage(const void *lpPkg, unsigned long nBytes)
{
  if(_bActive)
   {
     if(_PackageType==cptCompatiblePkg)
      {
        if(_QueueWrite->In(lpPkg))
         {
           _WritePackage();
           return 1;
         }
      }
     else
      {
        unsigned long n = 0;
        char *Buf = new char[_QueueWrite->PkgSize];
        try
         {
           if(nBytes && lpPkg)
            {
              ((TCommPackageHead*)Buf)->Length = nBytes<_PackageDataSize ? nBytes : _PackageDataSize;
              memcpy(((TCommPackageHead*)Buf)->Data, lpPkg, ((TCommPackageHead*)Buf)->Length);
              if(_QueueWrite->In(Buf))
               {
                 _WritePackage();
                 n = ((TCommPackageHead*)Buf)->Length;
               }
            }
         }
        __finally
         {
           delete[] Buf;
         }
        return n;
      }
   }
  return 0;
}
//---------------------------------------------------------------------------
void __fastcall TCommPackage::PurgeRead(void)
{
  if(_bActive)
   {
     _CommPort->PurgeRead();
     _QueueRead->Clear();
   }
}
//---------------------------------------------------------------------------
void __fastcall TCommPackage::PurgeWrite(void)
{
  if(_bActive)
   {
     _QueueWrite->Clear();
     _CommPort->PurgeWrite();
   }
}
//---------------------------------------------------------------------------
void __fastcall TCommPackage::_TimerThread(void)
{
  DWORD dwWait;
  try
   {
     _bTimerThreadRunning = true;
     while(_bRunTimerThread)
      {
        dwWait = (_bActive && _PackageType==cptFrameTimeout) ? _dwTimerInterval : INFINITE;
        WaitForSingleObject(_hTimerEvent,dwWait);

        if((_bActive&&_bRunTimerThread) && (_PackageType==cptFrameTimeout))
         {
           if((!_ReadTimeoutPackage()) && (_BufferRead->Length>0))
            {
              _QueueRead->In(_BufferRead);
              _BufferRead->Length=0;
              CallNotify(EV_RXCHAR);
            }
         }
      }
   }
  __finally
   {
     _bTimerThreadRunning = false;
   }
}
//---------------------------------------------------------------------------
unsigned long __fastcall TCommPackage::_EncodePackage(TCommPackageHead *lpEncoded, TCommPackageHead *lpPackage) //编码，目标包括头、尾
{
  unsigned long w = 0;
  unsigned long sn = lpPackage->Length < _PackageDataSize ? lpPackage->Length : _PackageDataSize;

  if(_bDoubleFrameMark)
    lpEncoded->Data[w++] = _FRAMEHEAD;
  lpEncoded->Data[w++] = _FRAMEHEAD;

  for(unsigned long r=0; r<sn; r++)
   {
     if((lpPackage->Data[r] == _FRAMEHEAD)
      ||(lpPackage->Data[r] == _FRAMETAIL)
      ||(lpPackage->Data[r] == _FRAMECTRL))
      {
        lpEncoded->Data[w++] = _FRAMECTRL;
      }
     lpEncoded->Data[w++] = lpPackage->Data[r];
   }

  if(_bDoubleFrameMark)
    lpEncoded->Data[w++] = _FRAMETAIL;
  lpEncoded->Data[w++] = _FRAMETAIL;

  lpEncoded->Length = w;
  return lpEncoded->Length;
}
//---------------------------------------------------------------------------
void __fastcall TCommPackage::_ReadHeadTailPackage(void)
{
  const int BufSize = 2048;
  unsigned char Buf[BufSize];
  int n;

  while((n=_CommPort->Read(Buf,BufSize)) > 0)
   {
     for(int i=0; i<n; i++)
      {
        if((!_bControlDetected) && (Buf[i]==_FRAMEHEAD)) //Synchronize
         {
           _BufferRead->Length = 0;
           _bFrameHeadDetected = true;
         }
        else if(_bFrameHeadDetected) //Store Package Data
         {
           if(_bControlDetected)
            {
              if(_BufferRead->Length<_PackageDataSize)
                _BufferRead->Data[_BufferRead->Length++] = Buf[i];
              _bControlDetected = false;
            }
           else if(Buf[i]==_FRAMECTRL)
            {
              _bControlDetected = true;
            }
           else if(Buf[i]==_FRAMETAIL)
            {
              _QueueRead->In(_BufferRead);
              _BufferRead->Length = 0;
              _bFrameHeadDetected = false;
            }
           else
            {
              if(_BufferRead->Length<_PackageDataSize)
                _BufferRead->Data[_BufferRead->Length++] = Buf[i];
              _bControlDetected = false;
            }
         }
        else //Detect & Ignore Data
         {
           if((!_bControlDetected) && (Buf[i]==_FRAMECTRL))
             _bControlDetected = true;
           else //Ignore data
             _bControlDetected = false;
         }
      }
   }
}
//---------------------------------------------------------------------------
bool __fastcall TCommPackage::_ReadTimeoutPackage(void)
{
  const int BufSize = 2048;
  unsigned char Buf[BufSize];
  int nRead, nFree;
  bool bReceived = false;

  while((nRead=_CommPort->Read(Buf,BufSize)) > 0)
   {
     nFree = _PackageDataSize - _BufferRead->Length;
     if(nRead>nFree)nRead=nFree;
     memcpy(_BufferRead->Data+_BufferRead->Length, Buf, nRead);
     _BufferRead->Length+=nRead;
     bReceived = true;
   }
  return bReceived;
}
//---------------------------------------------------------------------------
void __fastcall TCommPackage::_WritePackage(void)
{
  if(!_bWritingPackage)
   {
     try
      {
        _bWritingPackage = true;

        if(_BufferBytesWrite < _BufferWrite->Length)
         {
           _BufferBytesWrite += _CommPort->Write(_BufferWrite->Data + _BufferBytesWrite, _BufferWrite->Length - _BufferBytesWrite);
         }
        else if(_QueueWrite->Count)
         {
           if(_PackageType!=cptFrameTimeout)
            {
              _QueueWrite->Out(_BufferTemp);
              _EncodePackage(_BufferWrite, _BufferTemp);
            }
           else
            {
              _QueueWrite->Out(_BufferWrite);
            }
           _BufferBytesWrite = _CommPort->Write(_BufferWrite->Data, _BufferWrite->Length);
         }
      }
     __finally
      {
        _bWritingPackage = false;
      }
   }
}
//---------------------------------------------------------------------------
} // namespace VictorComm
} // namespace Vcl
//---------------------------------------------------------------------------
