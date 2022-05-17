/***************************************************************************\
*                                                                           *
*      Vcl.VictorFSM.cpp - Victor Finite State Machine, Version 1.5.0.6     *
*                            Built: May. 8, 2017                            *
*                     Copyright © 1997-2017, Victor Chen                    *
*                      Homepage: http://www.cppfans.com                     *
*                          Email: victor@cppfans.com                        *
*                                                                           *
\***************************************************************************/

#pragma hdrstop
#include "Vcl.VictorFSM.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
namespace Vcl {
namespace VictorClasses {
//---------------------------------------------------------------------------
//////////////////// TVictorFsmTimer::TVictorTimerThread ////////////////////
//---------------------------------------------------------------------------
class TVictorFsmTimer::TVictorTimerThread : public TThread
{
public:
  __property DWORD  Interval    = { read = _dwInterval, write = fSetInterval };
  __property HANDLE EventHandle = { read = _hTimerEvent };
  __fastcall TVictorTimerThread(HANDLE hTimerEvent);
  __fastcall ~TVictorTimerThread();
protected:
  virtual void __fastcall Execute(void);
private:
  DWORD _dwInterval;
  HANDLE _hTimerEvent, _hInternalEvent;
  bool _bQuitting;
  void fSetInterval(DWORD dwIntv);
};
//---------------------------------------------------------------------------
__fastcall TVictorFsmTimer::TVictorTimerThread::TVictorTimerThread(HANDLE hTimerEvent)
  : TThread(false)
{
  _dwInterval = 100;
  _hTimerEvent = hTimerEvent;
  _hInternalEvent = NULL;
  _bQuitting = false;
}
//---------------------------------------------------------------------------
__fastcall TVictorFsmTimer::TVictorTimerThread::~TVictorTimerThread()
{
  _bQuitting = true;
  if(_hInternalEvent)
    SetEvent(_hInternalEvent);
}
//---------------------------------------------------------------------------
void TVictorFsmTimer::TVictorTimerThread::fSetInterval(DWORD dwIntv)
{
  _dwInterval = dwIntv;
  if(_hInternalEvent)
    SetEvent(_hInternalEvent);
}
//---------------------------------------------------------------------------
void __fastcall TVictorFsmTimer::TVictorTimerThread::Execute(void)
{
  _hInternalEvent = CreateEvent(NULL,true,false,NULL);
  while(!Terminated && !_bQuitting)
   {
     WaitForSingleObject(_hInternalEvent,_dwInterval); // WAIT_OBJECT_0, WAIT_TIMEOUT
     ResetEvent(_hInternalEvent);
     if(_hTimerEvent)
       SetEvent(_hTimerEvent);
   }
  CloseHandle(_hInternalEvent);
  _hInternalEvent = NULL;
}
//---------------------------------------------------------------------------
////////////////////////////// TVictorFsmTimer //////////////////////////////
//---------------------------------------------------------------------------
__fastcall TVictorFsmTimer::TVictorFsmTimer()
  : TThread(false)
{
  _lpfnTimer = NULL;
  _lpfnOnStart = NULL;
  _lpfnOnStop = NULL;
  _dwWaitms = 0;
  _bQuitting = false;

  _hTimerEvent = CreateEvent(NULL,true,false,NULL);
  _hWaitEvent  = CreateEvent(NULL,true,false,NULL);

  _TimerThread = new TVictorTimerThread(_hTimerEvent);
}
//---------------------------------------------------------------------------
__fastcall TVictorFsmTimer::~TVictorFsmTimer()
{
  _bQuitting = true;

  SetEvent(_hWaitEvent);
  SetEvent(_hTimerEvent);

  delete _TimerThread;

  CloseHandle(_hWaitEvent);
  CloseHandle(_hTimerEvent);
}
//---------------------------------------------------------------------------
DWORD __fastcall TVictorFsmTimer::_fGetInterval(void)const
{
  return _TimerThread->Interval;
}
//---------------------------------------------------------------------------
void __fastcall TVictorFsmTimer::_fSetInterval(DWORD dwIntv)
{
  _TimerThread->Interval = dwIntv;
}
//---------------------------------------------------------------------------
void __fastcall TVictorFsmTimer::Trigger(void)
{
  _dwWaitms = 0;
  SetEvent(_hTimerEvent);
  SetEvent(_hWaitEvent);
}
//---------------------------------------------------------------------------
void __fastcall TVictorFsmTimer::Wait(DWORD dwms)
{
  _dwWaitms = dwms;
  SetEvent(_hTimerEvent);
  SetEvent(_hWaitEvent);
}
//---------------------------------------------------------------------------
void __fastcall TVictorFsmTimer::Execute(void)
{
  Synchronize(EvStart);
  while(!_bQuitting && !Terminated)
   {
     if(_dwWaitms)
      {
        DWORD dwWait = _dwWaitms;
        _dwWaitms = 0;
        WaitForSingleObject(_hWaitEvent, dwWait);
      }
     else
      {
        WaitForSingleObject(_hTimerEvent, Interval*2);
      }

     ResetEvent(_hTimerEvent);
     ResetEvent(_hWaitEvent);

     if(!_bQuitting && !Terminated && !_dwWaitms)
      {
        Synchronize(EvTimer);
        if(_dwWaitms)
         {
           ResetEvent(_hWaitEvent);
         }
      }
   }
  Synchronize(EvStop);
}
//---------------------------------------------------------------------------
void __fastcall TVictorFsmTimer::EvStart(void)
{
  if(_lpfnOnStart)_lpfnOnStart(this);
}
//---------------------------------------------------------------------------
void __fastcall TVictorFsmTimer::EvStop(void)
{
  if(_lpfnOnStop)_lpfnOnStop(this);
}
//---------------------------------------------------------------------------
void __fastcall TVictorFsmTimer::EvTimer(void)
{
  if(_lpfnTimer)_lpfnTimer(this);
}
//---------------------------------------------------------------------------
////////////////////////////// TVictorCustomFSM /////////////////////////////
//---------------------------------------------------------------------------
__fastcall TVictorCustomFSM::TVictorCustomFSM(TComponent *Owner)
 : TComponent(Owner)
{
  _lpfnProcessState = NULL;
  _bSetWaitState = false;

  _FsmState = 0;
  _FsmState.OnSetState = _FsmSetState;

  _FsmTimer = new TVictorFsmTimer;
  _FsmTimer->OnTimer = _FsmThreadTimer;
}
//---------------------------------------------------------------------------
__fastcall TVictorCustomFSM::~TVictorCustomFSM()
{
  if(_FsmTimer)
   {
     delete _FsmTimer;
     _FsmTimer = NULL;
   }
}
//---------------------------------------------------------------------------
int __fastcall TVictorCustomFSM::fGetState(void)
{
  return _FsmState;
}
//---------------------------------------------------------------------------
void __fastcall TVictorCustomFSM::fSetState(int iState)
{
  _FsmState = iState;
}
//---------------------------------------------------------------------------
DWORD __fastcall TVictorCustomFSM::fGetElapsed(void)
{
  return _FsmState.Elapsed;
}
//---------------------------------------------------------------------------
void __fastcall TVictorCustomFSM::fSetElapsed(DWORD dw)
{
  _FsmState.Elapsed = dw;
}
//---------------------------------------------------------------------------
DWORD __fastcall TVictorCustomFSM::fGetWaiting(void)
{
  return _FsmTimer->Waiting;
}
//---------------------------------------------------------------------------
DWORD __fastcall TVictorCustomFSM::fGetInterval(void)
{
  return  _FsmTimer->Interval;
}
//---------------------------------------------------------------------------
void  __fastcall TVictorCustomFSM::fSetInterval(DWORD dw)
{
  _FsmTimer->Interval = dw;
}
//---------------------------------------------------------------------------
void  __fastcall TVictorCustomFSM::_FsmSetState(TVictorFsmState *Sender, int &iNewState, int iOldState)
{
  if(!_bSetWaitState)
   {
     _FsmTimer->Trigger();
   }
}
//---------------------------------------------------------------------------
void __fastcall TVictorCustomFSM::_FsmThreadTimer(TObject *Sender)
{
  ProcessState();
}
//---------------------------------------------------------------------------
void __fastcall TVictorCustomFSM::ProcessState(void)
{
  if(_lpfnProcessState)_lpfnProcessState(this);
}
//---------------------------------------------------------------------------
void __fastcall TVictorCustomFSM::Trigger(void)
{
  if(_FsmTimer){ _FsmTimer->Trigger(); }
}
//---------------------------------------------------------------------------
void __fastcall TVictorCustomFSM::Wait(DWORD dwms)
{
  if(_FsmTimer){ _FsmTimer->Wait(dwms); }
}
//---------------------------------------------------------------------------
void __fastcall TVictorCustomFSM::WaitState(int iState, DWORD dwms)
{
  try
   {
     _bSetWaitState = true;
     _FsmState = iState;
     _FsmTimer->Wait(dwms);
   }
  __finally
   {
     _bSetWaitState = false;
   }
}
//---------------------------------------------------------------------------
///////////////////////////////// TVictorFSM ////////////////////////////////
//---------------------------------------------------------------------------
__fastcall TVictorFSM::TVictorFSM(TComponent *Owner)
 : TVictorCustomFSM(Owner)
{
}
//---------------------------------------------------------------------------
} // namespace VictorClasses
} // namespace Vcl
//---------------------------------------------------------------------------
