/***************************************************************************\
*                                                                           *
*       Vcl.VictorFSM.h - Victor Finite State Machine, Version 1.5.0.6      *
*                            Built: May. 8, 2017                            *
*                     Copyright © 1997-2017, Victor Chen                    *
*                      Homepage: http://www.cppfans.com                     *
*                          Email: victor@cppfans.com                        *
*                                                                           *
\***************************************************************************/

#ifndef Vcl_VictorFSMH
#define Vcl_VictorFSMH
//---------------------------------------------------------------------------
#include <vcl.h>
//---------------------------------------------------------------------------
namespace Vcl {
namespace VictorClasses {
//---------------------------------------------------------------------------
template <class TStateType> class TVictorFsmStateT
{
public:
  typedef void __fastcall (__closure *TSetState)(TVictorFsmStateT *Sender, TStateType &stNewState, TStateType stOldState);

private:
  TStateType _stState      ;
  DWORD      _iStartTime   ;
  TSetState  _lpfnSetState ;

  inline DWORD __fastcall fGetCounter(void) const          { DWORD t=GetTickCount(); return t-_iStartTime; }
  inline void  __fastcall fSetCounter(DWORD c)             { DWORD t=GetTickCount(); _iStartTime=t-c;      }
  inline void __fastcall  fSetState  (const TStateType st) { TStateType OldST=_stState; _stState=st; if(_lpfnSetState){ _lpfnSetState(this,_stState,OldST); } _iStartTime=GetTickCount(); }

public:
  __property TStateType State      = { read = _stState      , write = fSetState     };
  __property DWORD      Elapsed    = { read = fGetCounter   , write = fSetCounter   };
  __property TSetState  OnSetState = { read = _lpfnSetState , write = _lpfnSetState };

  inline operator       TStateType()       { return _stState; }
  inline operator const TStateType() const { return _stState; }

  inline const TVictorFsmStateT &operator = (const TStateType st){ fSetState(st); return *this; }
  inline __fastcall TVictorFsmStateT() { _lpfnSetState=NULL; State = (TStateType)(0); }
};
//---------------------------------------------------------------------------
typedef TVictorFsmStateT <int> TVictorFsmState;
//---------------------------------------------------------------------------
class PACKAGE TVictorFsmTimer : public TThread
{
public:
  __property DWORD        Interval = { read = _fGetInterval, write = _fSetInterval };
  __property DWORD        Waiting  = { read = _dwWaitms                            };
  __property TNotifyEvent OnTimer  = { read = _lpfnTimer   , write = _lpfnTimer    };
  __property TNotifyEvent OnStart  = { read = _lpfnOnStart , write = _lpfnOnStart  };
  __property TNotifyEvent OnStop   = { read = _lpfnOnStop  , write = _lpfnOnStop   };

  virtual void __fastcall Trigger(void);
  virtual void __fastcall Wait(DWORD dwms);

  __fastcall TVictorFsmTimer();
  __fastcall ~TVictorFsmTimer();

protected:
  virtual void __fastcall Execute(void);
  virtual void __fastcall EvStart(void);
  virtual void __fastcall EvStop(void);
  virtual void __fastcall EvTimer(void);

private:
  HANDLE _hTimerEvent, _hWaitEvent;
  class TVictorTimerThread;
  TVictorTimerThread *_TimerThread;
  TNotifyEvent _lpfnTimer, _lpfnOnStart, _lpfnOnStop;
  DWORD _dwWaitms;
  bool _bQuitting;
  DWORD __fastcall _fGetInterval(void)const;
  void __fastcall _fSetInterval(DWORD dwIntv);
};
//---------------------------------------------------------------------------
class PACKAGE TVictorCustomFSM : public TComponent
{
private:
  TVictorFsmState _FsmState;
  TVictorFsmTimer *_FsmTimer;
  TNotifyEvent _lpfnProcessState;
  bool _bSetWaitState;

  int   __fastcall fGetState(void);
  void  __fastcall fSetState(int iState);
  DWORD __fastcall fGetElapsed(void);
  void  __fastcall fSetElapsed(DWORD dw);
  DWORD __fastcall fGetWaiting(void);
  DWORD __fastcall fGetInterval(void);
  void  __fastcall fSetInterval(DWORD dw);

  void  __fastcall _FsmSetState(TVictorFsmState *Sender, int &iNewState, int iOldState);
  void  __fastcall _FsmThreadTimer(TObject *Sender);

public:
  __property int   State    = { read = fGetState    , write = fSetState    };
  __property DWORD Elapsed  = { read = fGetElapsed  , write = fSetElapsed  };
  __property DWORD Waiting  = { read = fGetWaiting                         };
  __property DWORD Interval = { read = fGetInterval , write = fSetInterval };

  __property TNotifyEvent OnProcessState = { read =  _lpfnProcessState , write = _lpfnProcessState };

  virtual void __fastcall Trigger(void);
  virtual void __fastcall Wait(DWORD dwms);
  virtual void __fastcall WaitState(int iState, DWORD dwms);
  virtual void __fastcall ProcessState(void);

  __fastcall TVictorCustomFSM(TComponent *Owner);
  __fastcall ~TVictorCustomFSM();
};
//---------------------------------------------------------------------------
class PACKAGE TVictorFSM : public TVictorCustomFSM
{
public:
   __fastcall TVictorFSM(TComponent *Owner);

   __property State;
   __property Elapsed;
   __property Waiting;

__published:
   __property Interval =  { default = 100 };
   __property OnProcessState;
};
//---------------------------------------------------------------------------
} // namespace VictorClasses
} // namespace Vcl
//---------------------------------------------------------------------------
using namespace Vcl::VictorClasses;
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
