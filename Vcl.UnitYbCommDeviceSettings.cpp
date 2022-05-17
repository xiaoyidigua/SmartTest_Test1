/***************************************************************************\
*                                                                           *
*  Vcl.UnitYbCommDeviceSettings.cpp - 串行口通讯参数设定窗口, 版本 1.5.0.6  *
*                             Built: Apr. 18, 2017                          *
*                     Copyright © 1997-2017, Victor Chen                    *
*                      Homepage: http://www.cppfans.com                     *
*                          Email: victor@cppfans.com                        *
*                                                                           *
\***************************************************************************/

#include <vcl.h>
#pragma hdrstop

#include "Vcl.YbCommDevice.h"
#include "Vcl.UnitYbCommDeviceSettings.h"
#include "Vcl.UnitYBCommDeviceAbout.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
namespace Vcl {
namespace VictorComm {
//---------------------------------------------------------------------------
// TForm_YbCommDevice_Settings *Form_YbCommDevice_Settings;
//---------------------------------------------------------------------------
__fastcall TForm_YbCommDevice_Settings::TForm_YbCommDevice_Settings(TComponent* Owner, TYbCustomCommDevice *lpCommDevice, bool FActive)
    : TForm(Owner)
{
  _YbCommDevice = lpCommDevice;
  _ForceActive  = FActive;

  TMsgStrings Msg;
  Caption             = Msg.MsgPortSet  ; //"串行口设置"
  GboxBase->Caption   = Msg.StrGbxBase  ; //"串行口选项"
  GBoxModem->Caption  = Msg.StrGbxModem ; //"调制解调器选项"
  GBoxBuffer->Caption = Msg.StrGbxBuffer; //"缓存容量"
  BnAbout->Caption    = Msg.MsgAbout    ; //"关于"
  BnDefault->Caption  = Msg.MsgDefault  ; //"默认"
  BnOK->Caption       = Msg.MsgOK       ; //"确定"
  BnCancel->Caption   = Msg.MsgCancel   ; //"取消"

  lbPort ->Caption = Msg.MsgPort  ; //"串行口"
  lbBaud ->Caption = Msg.MsgBaud  ; //"波特率"
  lbCheck->Caption = Msg.MsgCheck ; //"校验"
  lbBits ->Caption = Msg.MsgBits  ; //"数据位"
  lbStops->Caption = Msg.MsgStops ; //"停止位"
  lbFlow ->Caption = Msg.MsgFlow  ; //"数据流控"
  lbAAns ->Caption = Msg.MsgAAns  ; //"自动应答"
  lbInbs ->Caption = Msg.MsgInBuf ; //"输入缓存"
  lbOutbs->Caption = Msg.MsgOutBuf; //"输出缓存"

  CbPort->ItemHeight = Canvas->TextHeight(L"Wg")+2;
  CbPort->Items->Clear();
  for(int i=0; i<_spi.PortList->Count; i++)
   {
     TVictorSerialPortInfo *lpSPInfo = _spi.PortList->Ports[i];
     CbPort->Items->Add(lpSPInfo->PortName);
   }

  CbParity->Items->Clear();
  CbParity->Items->Add(Msg.MsgNoParity); //"No parity (无)"
  CbParity->Items->Add(Msg.MsgOdd     ); //"Odd (奇)"
  CbParity->Items->Add(Msg.MsgEven    ); //"Even (偶)"
  CbParity->Items->Add(Msg.MsgMark    ); //"Mark (标记)"
  CbParity->Items->Add(Msg.MsgSpacePty); //"Space (空格)"

  CbFlow->Items->Clear();
  CbFlow->Items->Add(Msg.MsgNone   ); //"None (无)"
  CbFlow->Items->Add(Msg.MsgRtsCts ); //"RTS/CTS (硬件)"
  CbFlow->Items->Add(Msg.MsgXonXoff); //"Xon/Xoff (软件)"
  CbFlow->Items->Add(Msg.MsgRtsXon ); //"RTS/CTS & Xon/Xoff (硬件和软件)"

  if(!_YbCommDevice->ComponentState.Contains(csDesigning))
   {
     BnAbout->Visible = false;
     BnDefault->Visible = false;
     BnOK->Left = (Width-BnOK->Width)/2 - 60;
     BnCancel->Left = (Width-BnCancel->Width)/2 + 60;
   }

  _OldActive      = _YbCommDevice->Active;
  _OldPortName    = _YbCommDevice->PortName;
  _OldBaud        = _YbCommDevice->CommPort->Baud;
  _OldByteSize    = _YbCommDevice->ByteSize;
  _OldParity      = _YbCommDevice->Parity;
  _OldStopBits    = _YbCommDevice->StopBits;
  _OldFlowControl = _YbCommDevice->FlowControl;
  _OldAutoAAns    = _YbCommDevice->AutoAnswer;
  _OldInBufSize   = _YbCommDevice->InBufSize;
  _OldOutBufSize  = _YbCommDevice->OutBufSize;

  CbPort    ->ItemIndex = CbPort->Items->IndexOf(_OldPortName);
  CbSelBaud ->Text      = _OldBaud;
  CbByteSize->ItemIndex = _OldByteSize-5;

  switch(_OldParity)
   {
     case TYbCommDevice::ptNoParity   : CbParity->ItemIndex = 0; break;
     case TYbCommDevice::ptOddParity  : CbParity->ItemIndex = 1; break;
     case TYbCommDevice::ptEvenParity : CbParity->ItemIndex = 2; break;
     case TYbCommDevice::ptMarkParity : CbParity->ItemIndex = 3; break;
     case TYbCommDevice::ptSpaceParity: CbParity->ItemIndex = 4; break;
     default                          : CbParity->ItemIndex =-1; break;
   }

  switch(_OldStopBits)
   {
     case TYbCommDevice::sbOneStopBit    : CbStopBits->ItemIndex = 0; break;
     case TYbCommDevice::sbOne_5_StopBits: CbStopBits->ItemIndex = 1; break;
     case TYbCommDevice::sbTwoStopBit    : CbStopBits->ItemIndex = 2; break;
     default                             : CbStopBits->ItemIndex =-1; break;
   }

  switch(_OldFlowControl)
   {
     case TYbCommDevice::fcRtsCts:        CbFlow->ItemIndex = 1; break; //Rts/Cts
     case TYbCommDevice::fcXonXoff:       CbFlow->ItemIndex = 2; break; //Xon/Xoff
     case TYbCommDevice::fcTranXonXoff:   CbFlow->ItemIndex = 2; break; //Xon/Xoff
     case TYbCommDevice::fcRtsCtsXonXoff: CbFlow->ItemIndex = 3; break; //Both
     default:                             CbFlow->ItemIndex = 0; break; //None
   }

  EditAutoAns->Text = (int)_OldAutoAAns;
  EditInBufSize->Text = _OldInBufSize;
  EditOutBufSize->Text = _OldOutBufSize;
}
//---------------------------------------------------------------------------
void __fastcall TForm_YbCommDevice_Settings::BnAboutClick(TObject *Sender)
{
  TForm_YBComm64_About *FormAbout = new TForm_YBComm64_About(NULL,_YbCommDevice->ClassName());
  FormAbout->ShowModal();
  delete FormAbout;
}
//---------------------------------------------------------------------------
void __fastcall TForm_YbCommDevice_Settings::BnDefaultClick(TObject *Sender)
{
  CbSelBaud->ItemIndex  = 12;   //TYbCommDevice::br115200;
  CbParity->ItemIndex   = 0;    //TYbCommDevice::ptNoParity;
  CbByteSize->ItemIndex = 8 - 5;
  CbStopBits->ItemIndex = 0;    //TYbCommDevice::sbOneStopBit;
  CbFlow->ItemIndex     = 0;    //TYbCommDevice::fcNone;
  EditAutoAns->Text     = 0;    //No Auto Answer
  EditInBufSize->Text   = 8192; //8kbytes
  EditOutBufSize->Text  = 8192; //8kbytes
}
//---------------------------------------------------------------------------
void __fastcall TForm_YbCommDevice_Settings::BnOKClick(TObject *Sender)
{
  if(MoveCtrlFocus())
    return; //move focus

  UnicodeString NewPortName;
  int   NewByteSize, NewAutoAAns;
  long  NewInBufSize, NewOutBufSize;
  DWORD NewBaud;
  TYbCommDevice::TParity    NewParity;
  TYbCommDevice::TStopBits   NewStopBits;
  TYbCommDevice::TFlowControl NewFlowControl;
  UnicodeString s;
  TMsgStrings Msg;

  //选择串行口
  if(CbPort->ItemIndex<0)
   {
     CbPort->SetFocus(); //"必须选择串行口"
     MessageBox(Handle, Msg.ErrSelPort.c_str(), Caption.c_str(), MB_OK|MB_ICONSTOP);
     return;
   }
  NewPortName = CbPort->Items->Strings[CbPort->ItemIndex];

  //设置波特率
  UnicodeString sNewBaud = CbSelBaud->Text.Trim();
  if(sNewBaud.IsEmpty())
   {
     CbSelBaud->SetFocus(); //"必须输入/选择波特率"
     MessageBox(Handle, Msg.ErrSelBaud.c_str(), Caption.c_str(), MB_OK|MB_ICONSTOP);
     return;
   }
  NewBaud = _wtoi(sNewBaud.c_str());

  //数据位数
  if(CbByteSize->ItemIndex<0)
   {
     CbByteSize->SetFocus(); //"必须选择数据位"
     MessageBox(Handle, Msg.ErrSelBits.c_str(), Caption.c_str(), MB_OK|MB_ICONSTOP);
     return;
   }
  NewByteSize = CbByteSize->ItemIndex+5;

  //校验位
  switch(CbParity->ItemIndex)
   {
     case  0: NewParity = TYbCommDevice::ptNoParity   ; break;
     case  1: NewParity = TYbCommDevice::ptOddParity  ; break;
     case  2: NewParity = TYbCommDevice::ptEvenParity ; break;
     case  3: NewParity = TYbCommDevice::ptMarkParity ; break;
     case  4: NewParity = TYbCommDevice::ptSpaceParity; break;
     default: CbParity->SetFocus(); //"必须选择奇偶校验"
              MessageBox(Handle, Msg.ErrSelCheck.c_str(), Caption.c_str(), MB_OK|MB_ICONSTOP);
              return;
   }

  //停止位
  switch(CbStopBits->ItemIndex)
   {
     case  0: NewStopBits = TYbCommDevice::sbOneStopBit    ; break;
     case  1: NewStopBits = TYbCommDevice::sbOne_5_StopBits; break;
     case  2: NewStopBits = TYbCommDevice::sbTwoStopBit    ; break;
     default: CbStopBits->SetFocus(); //"必须选择停止位"
              MessageBox(Handle, Msg.ErrSelStops.c_str(), Caption.c_str(), MB_OK|MB_ICONSTOP);
              return;
   }

  //数据流控
  switch(CbFlow->ItemIndex)
   {
     case  0: NewFlowControl = TYbCommDevice::fcNone         ; break;
     case  1: NewFlowControl = TYbCommDevice::fcRtsCts       ; break;
     case  2: NewFlowControl = TYbCommDevice::fcXonXoff      ; break;
     case  3: NewFlowControl = TYbCommDevice::fcRtsCtsXonXoff; break;
     default: CbFlow->SetFocus(); //"必须选择数据流控制"
              MessageBox(Handle, Msg.ErrSelFlow.c_str(), Caption.c_str(), MB_OK|MB_ICONSTOP);
              return;
   }

  //自动应答
  try
   {
     s = EditAutoAns->Text.Trim();
     if(s.IsEmpty())
       throw Exception(Msg.ErrInvAAns);
     int ians = StrToInt(s);
     if((ians<0)||(ians>255))
       throw Exception(Msg.ErrInvAAns);
     NewAutoAAns = ians;
   }
  catch(Exception &e)
   {
     EditAutoAns->SetFocus();
     MessageBox(Handle, Msg.ErrInvAAns.c_str(),Caption.c_str(),MB_OK|MB_ICONSTOP);
     return;
   }

  //输入缓存
  try
   {
     s = EditInBufSize->Text.Trim();
     if(s.IsEmpty())
       throw Exception(Msg.ErrInvInBuf);
     NewInBufSize = StrToInt(s);
     if(NewInBufSize<4096)
       throw Exception(Msg.ErrInvInBuf);
   }
  catch(Exception &e)
   {
     EditInBufSize->SetFocus();
     MessageBox(Handle, Msg.ErrInvInBuf.c_str(),Caption.c_str(),MB_OK|MB_ICONSTOP);
     return;
   }

  //输出缓存
  try
   {
     s = EditOutBufSize->Text.Trim();
     if(s.IsEmpty())
       throw Exception(Msg.ErrInvOutBuf);
     NewOutBufSize = StrToInt(s);
     if(NewOutBufSize<4096)
       throw Exception(Msg.ErrInvOutBuf);
   }
  catch(Exception &e)
   {
     EditOutBufSize->SetFocus();
     MessageBox(Handle, Msg.ErrInvOutBuf.c_str(),Caption.c_str(),MB_OK|MB_ICONSTOP);
     return;
   }

  //设置参数
  try
   {
     _YbCommDevice->PortName       = NewPortName;
     _YbCommDevice->CommPort->Baud = NewBaud;
     _YbCommDevice->Parity         = NewParity;
     _YbCommDevice->ByteSize       = NewByteSize;
     _YbCommDevice->StopBits       = NewStopBits;
     _YbCommDevice->FlowControl    = NewFlowControl;
     _YbCommDevice->AutoAnswer     = NewAutoAAns;
     _YbCommDevice->InBufSize      = NewInBufSize;
     _YbCommDevice->OutBufSize     = NewOutBufSize;
     _YbCommDevice->Active         = _OldActive||_ForceActive;
     ModalResult = IDOK;
   }
  catch(Exception &e)
   {
     try
      {
        _YbCommDevice->PortName       = _OldPortName;
        _YbCommDevice->CommPort->Baud = _OldBaud;
        _YbCommDevice->Parity         = _OldParity;
        _YbCommDevice->ByteSize       = _OldByteSize;
        _YbCommDevice->StopBits       = _OldStopBits;
        _YbCommDevice->FlowControl    = _OldFlowControl;
        _YbCommDevice->AutoAnswer     = _OldAutoAAns;
        _YbCommDevice->InBufSize      = _OldInBufSize;
        _YbCommDevice->OutBufSize     = _OldOutBufSize;
        _YbCommDevice->Active         = _OldActive;
      }
     catch(...)
      {
        //do nothing, ignore this error
      }
     CbPort->SetFocus();
     MessageBox(Handle, e.Message.c_str(), Caption.c_str(), MB_OK|MB_ICONSTOP);
   }
}
//---------------------------------------------------------------------------
bool __fastcall TForm_YbCommDevice_Settings::MoveCtrlFocus(void)
{
  if(ActiveControl == CbPort       ){ ActiveControl = CbSelBaud     ; return true; }
  if(ActiveControl == CbSelBaud    ){ ActiveControl = CbParity      ; return true; }
  if(ActiveControl == CbParity     ){ ActiveControl = CbByteSize    ; return true; }
  if(ActiveControl == CbByteSize   ){ ActiveControl = CbStopBits    ; return true; }
  if(ActiveControl == CbStopBits   ){ ActiveControl = CbFlow        ; return true; }
  if(ActiveControl == CbFlow       ){ ActiveControl = EditAutoAns   ; return true; }
  if(ActiveControl == EditAutoAns  ){ ActiveControl = EditInBufSize ; return true; }
  if(ActiveControl == EditInBufSize){ ActiveControl = EditOutBufSize; return true; }
  return false;
}
//---------------------------------------------------------------------------
void __fastcall TForm_YbCommDevice_Settings::BnCancelClick(TObject *Sender)
{
  ModalResult = IDCANCEL;
}
//---------------------------------------------------------------------------
void __fastcall TForm_YbCommDevice_Settings::CbPortDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State)
{
  TComboBox *lpcb = dynamic_cast<TComboBox *>(Control);
  if(lpcb)
   {
     TCanvas *lpCanvas = lpcb->Canvas;
     if(Index>=0 && Index<lpcb->Items->Count)
      {
        lpCanvas->FillRect(Rect);
        String s = lpcb->Items->Strings[Index];
        TRect rStrSize, r(Rect.Left+2,Rect.Top,Rect.Right-1,Rect.Bottom);
        ::DrawText(lpCanvas->Handle, s.c_str(), s.Length(), &rStrSize, DT_LEFT|DT_SINGLELINE|DT_TOP|DT_NOPREFIX|DT_CALCRECT);
        ::DrawText(lpCanvas->Handle, s.c_str(), s.Length(), &r, DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX);
        int iStrWidth = rStrSize.Width(); if(iStrWidth<55)iStrWidth=55;
        int iLinePosX = r.Left + iStrWidth + 1;

        COLORREF crBkgd = ColorToRGB(lpCanvas->Brush->Color);
        COLORREF crText = ColorToRGB(lpCanvas->Font->Color);
        COLORREF crLine = RGB((GetRValue(crText)+GetRValue(crBkgd))/2, (GetGValue(crText)+GetGValue(crBkgd))/2, (GetBValue(crText)+GetBValue(crBkgd))/2);

        TColor cOldPenColor = lpCanvas->Pen->Color;
        lpCanvas->Pen->Color = TColor(crLine);
        lpCanvas->MoveTo(iLinePosX, Rect.Top   );
        lpCanvas->LineTo(iLinePosX, Rect.Bottom);
        lpCanvas->Pen->Color = cOldPenColor;

        if(Index>=0 && Index<_spi.PortList->Count)
         {
           s = _spi.PortList->Ports[Index]->FriendlyName;
           r.Left = iLinePosX + 3;
           ::DrawText(lpCanvas->Handle, s.c_str(), s.Length(), &r, DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX);
         }
      }
   }
}
//---------------------------------------------------------------------------
//////////////////////////////// TMsgStrings ////////////////////////////////
//---------------------------------------------------------------------------
__fastcall TForm_YbCommDevice_Settings::TMsgStrings::TMsgStrings()
{
  static const wchar_t *_Msg_Chs[] =
   {
     L"确定"                         , // 0 MsgOK
     L"取消"                         , // 1 MsgCancel
     L"串行口设置"                   , // 2 MsgPortSet
     L"串行口"                       , // 3 MsgPort
     L"波特率"                       , // 4 MsgBaud
     L"奇偶校验"                     , // 5 MsgCheck
     L"数据位"                       , // 6 MsgBits
     L"停止位"                       , // 7 MsgStops
     L"数据流控"                     , // 8 MsgFlow
     L"No parity (无)"               , // 9 MsgNoParity
     L"Odd (奇)"                     , //10 MsgOdd
     L"Even (偶)"                    , //11 MsgEven
     L"Mark (标记)"                  , //12 MsgMark
     L"None (无)"                    , //13 MsgNone
     L"RTS/CTS (硬件)"               , //14 MsgRtsCts
     L"Xon/Xoff (软件)"              , //15 MsgXonXoff
     L"RTS/CTS & Xon/Xoff (硬&软)"   , //16 MsgRtsXon
     L"必须选择串行口"               , //17 ErrSelPort
     L"必须输入/选择波特率"          , //18 ErrSelBaud
     L"无效的波特率"                 , //19 ErrInvBaud
     L"必须选择数据位"               , //20 ErrSelBits
     L"必须选择奇偶校验"             , //21 ErrSelCheck
     L"必须选择停止位"               , //22 ErrSelStops
     L"必须选择数据流控制"           , //23 ErrSelFlow
     L"关于"                         , //24 MsgAbout
     L"默认"                         , //25 MsgDefault
     L"串行口选项"                   , //26 StrGbxBase
     L"调制解调器选项"               , //27 StrGbxModem
     L"缓存容量"                     , //28 StrGbxBuffer
     L"自动应答"                     , //29 MsgAAns
     L"输入缓存"                     , //30 MsgInBuf
     L"输出缓存"                     , //31 MsgOutBuf
     L"自动应答必须是 0 - 255 的整数", //32 ErrInvAAns
     L"输入缓存容量错误"             , //33 ErrInvInBuf
     L"输出缓存容量错误"             , //34 ErrInvOutBuf
     L"Space (空格)"                 , //35 MsgSpacePty
   };
  static const wchar_t *_Msg_Cht[] =
   {
     L"確定"                         , // 0 MsgOK
     L"取消"                         , // 1 MsgCancel
     L"序列埠設定"                   , // 2 MsgPortSet
     L"序列埠"                       , // 3 MsgPort
     L"傳輸速率"                     , // 4 MsgBaud
     L"奇偶校驗"                     , // 5 MsgCheck
     L"資料位元"                     , // 6 MsgBits
     L"停止位元"                     , // 7 MsgStops
     L"資料流控"                     , // 8 MsgFlow
     L"No parity (無)"               , // 9 MsgNoParity
     L"Odd (奇)"                     , //10 MsgOdd
     L"Even (偶)"                    , //11 MsgEven
     L"Mark (標記)"                  , //12 MsgMark
     L"None (無)"                    , //13 MsgNone
     L"RTS/CTS (硬體)"               , //14 MsgRtsCts
     L"Xon/Xoff (軟體)"              , //15 MsgXonXoff
     L"RTS/CTS & Xon/Xoff (硬&軟)"   , //16 MsgRtsXon
     L"必須選擇序列埠"               , //17 ErrSelPort
     L"必須輸入/選擇序列傳輸速率"    , //18 ErrSelBaud
     L"無效的序列傳輸速率"           , //19 ErrInvBaud
     L"必須選擇資料位元"             , //20 ErrSelBits
     L"必須選擇奇偶校驗"             , //21 ErrSelCheck
     L"必須選擇停止位元"             , //22 ErrSelStops
     L"必須選擇資料流程控制"         , //23 ErrSelFlow
     L"關於"                         , //24 MsgAbout
     L"默認"                         , //25 MsgDefault
     L"序列埠選項"                   , //26 StrGbxBase
     L"數據機選項"                   , //27 StrGbxModem
     L"緩存容量"                     , //28 StrGbxBuffer
     L"自動回應"                     , //29 MsgAAns
     L"輸入緩存"                     , //30 MsgInBuf
     L"輸出緩存"                     , //31 MsgOutBuf
     L"自動回應必須是 0 - 255 的整數", //32 ErrInvAAns
     L"輸入緩存容量錯誤"             , //33 ErrInvInBuf
     L"輸出緩存容量錯誤"             , //34 ErrInvOutBuf
     L"Space (空白)"                 , //35 MsgSpacePty
   };
  static const wchar_t *_Msg_Enu[] =
   {
     L"&OK"                       , // 0 MsgOK
     L"&Cancel"                   , // 1 MsgCancel
     L"Serial Port Setup"         , // 2 MsgPortSet
     L"Port"                      , // 3 MsgPort
     L"Baud"                      , // 4 MsgBaud
     L"Parity"                    , // 5 MsgCheck
     L"Byte Size"                 , // 6 MsgBits
     L"Stop Bits"                 , // 7 MsgStops
     L"Flow Control"              , // 8 MsgFlow
     L"No parity"                 , // 9 MsgNoParity
     L"Odd"                       , //10 MsgOdd
     L"Even"                      , //11 MsgEven
     L"Mark"                      , //12 MsgMark
     L"None"                      , //13 MsgNone
     L"RTS/CTS"                   , //14 MsgRtsCts
     L"Xon/Xoff"                  , //15 MsgXonXoff
     L"RTS/CTS & Xon/Xoff"        , //16 MsgRtsXon
     L"Must select COM Port"      , //17 ErrSelPort
     L"Must select baud rate"     , //18 ErrSelBaud
     L"Invalid baud rate"         , //19 ErrInvBaud
     L"Must select byte size"     , //20 ErrSelBits
     L"Must select parity check"  , //21 ErrSelCheck
     L"Must select stop bits"     , //22 ErrSelStops
     L"Must select flow control"  , //23 ErrSelFlow
     L"&About"                    , //24 MsgAbout
     L"&Default"                  , //25 MsgDefault
     L"Serial Port"               , //26 StrGbxBase
     L"Modem"                     , //27 StrGbxModem
     L"Buffer"                    , //28 StrGbxBuffer
     L"Auto Answer"               , //29 MsgAAns
     L"Input Buf."                , //30 MsgInBuf
     L"Output Buf."               , //31 MsgOutBuf
     L"Auto Answer: 0 - 255"      , //32 ErrInvAAns
     L"Input Buffer size error"   , //33 ErrInvInBuf
     L"Output Buffer size error"  , //34 ErrInvOutBuf
     L"Space Parity"              , //35 MsgSpacePty
   };

  switch(_CommLanguage.lcId)
   {
     case TVictorCommLanguage::lcidChs:
          _Msg = _Msg_Chs;
          break;

     case TVictorCommLanguage::lcidCht:
          _Msg = _Msg_Cht;
          break;

     default:
          _Msg = _Msg_Enu;
          break;
   }
}
//---------------------------------------------------------------------------
UnicodeString __fastcall TForm_YbCommDevice_Settings::TMsgStrings::fGetMsg(int mt)
{
  return _Msg[mt];
}
//---------------------------------------------------------------------------
} // namespace VictorComm
} // namespace Vcl
//---------------------------------------------------------------------------
