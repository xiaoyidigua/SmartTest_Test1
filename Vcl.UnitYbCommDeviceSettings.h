/***************************************************************************\
*                                                                           *
*   Vcl.UnitYbCommDeviceSettings.h - 串行口通讯参数设定窗口, 版本 1.5.0.6   *
*                             Built: Apr. 18, 2017                          *
*                     Copyright © 1997-2017, Victor Chen                    *
*                      Homepage: http://www.cppfans.com                     *
*                          Email: victor@cppfans.com                        *
*                                                                           *
\***************************************************************************/

#ifndef Vcl_UnitYbCommDeviceSettingsH
#define Vcl_UnitYbCommDeviceSettingsH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
//---------------------------------------------------------------------------
namespace Vcl {
namespace VictorComm {
//---------------------------------------------------------------------------
class PACKAGE TForm_YbCommDevice_Settings : public TForm
{
__published:	// IDE-managed Components
    TGroupBox *GboxBase;
    TLabel *lbBaud;
    TLabel *lbCheck;
    TLabel *lbBits;
    TLabel *lbStops;
    TLabel *lbPort;
    TComboBox *CbPort;
    TComboBox *CbSelBaud;
    TComboBox *CbParity;
    TComboBox *CbByteSize;
    TComboBox *CbStopBits;
    TGroupBox *GBoxModem;
    TLabel *lbFlow;
    TLabel *lbAAns;
    TComboBox *CbFlow;
    TEdit *EditAutoAns;
    TGroupBox *GBoxBuffer;
    TLabel *lbInbs;
    TLabel *lbOutbs;
    TEdit *EditInBufSize;
    TEdit *EditOutBufSize;
    TBitBtn *BnAbout;
    TBitBtn *BnDefault;
    TBitBtn *BnOK;
    TBitBtn *BnCancel;
    void __fastcall BnAboutClick(TObject *Sender);
    void __fastcall BnDefaultClick(TObject *Sender);
    void __fastcall BnOKClick(TObject *Sender);
    void __fastcall BnCancelClick(TObject *Sender);
    void __fastcall CbPortDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State);
public:		// User declarations
    class PACKAGE TMsgStrings
     {
     public:
       __property UnicodeString MsgOK       = { read = fGetMsg, index =  0}; //"确定"
       __property UnicodeString MsgCancel   = { read = fGetMsg, index =  1}; //"取消"
       __property UnicodeString MsgPortSet  = { read = fGetMsg, index =  2}; //"端口设置"
       __property UnicodeString MsgPort     = { read = fGetMsg, index =  3}; //"端口"
       __property UnicodeString MsgBaud     = { read = fGetMsg, index =  4}; //"波特率"
       __property UnicodeString MsgCheck    = { read = fGetMsg, index =  5}; //"校验"
       __property UnicodeString MsgBits     = { read = fGetMsg, index =  6}; //"数据位"
       __property UnicodeString MsgStops    = { read = fGetMsg, index =  7}; //"停止位"
       __property UnicodeString MsgFlow     = { read = fGetMsg, index =  8}; //"数据流控"
       __property UnicodeString MsgNoParity = { read = fGetMsg, index =  9}; //"No parity (无)"
       __property UnicodeString MsgOdd      = { read = fGetMsg, index = 10}; //"Odd (奇)"
       __property UnicodeString MsgEven     = { read = fGetMsg, index = 11}; //"Even (偶)"
       __property UnicodeString MsgMark     = { read = fGetMsg, index = 12}; //"Mark (标记)"
       __property UnicodeString MsgNone     = { read = fGetMsg, index = 13}; //"None (无)"
       __property UnicodeString MsgRtsCts   = { read = fGetMsg, index = 14}; //"RTS/CTS (硬件)"
       __property UnicodeString MsgXonXoff  = { read = fGetMsg, index = 15}; //"Xon/Xoff (软件)"
       __property UnicodeString MsgRtsXon   = { read = fGetMsg, index = 16}; //"RTS/CTS & Xon/Xoff (硬&软)"
       __property UnicodeString ErrSelPort  = { read = fGetMsg, index = 17}; //"必须选择端口"
       __property UnicodeString ErrSelBaud  = { read = fGetMsg, index = 18}; //"必须输入/选择波特率"
       __property UnicodeString ErrInvBaud  = { read = fGetMsg, index = 19}; //"无效的波特率"
       __property UnicodeString ErrSelBits  = { read = fGetMsg, index = 20}; //"必须选择数据位"
       __property UnicodeString ErrSelCheck = { read = fGetMsg, index = 21}; //"必须选择奇偶校验"
       __property UnicodeString ErrSelStops = { read = fGetMsg, index = 22}; //"必须选择停止位"
       __property UnicodeString ErrSelFlow  = { read = fGetMsg, index = 23}; //"必须选择数据流控制"
       __property UnicodeString MsgAbout    = { read = fGetMsg, index = 24}; //"关于"
       __property UnicodeString MsgDefault  = { read = fGetMsg, index = 25}; //"默认"
       __property UnicodeString StrGbxBase  = { read = fGetMsg, index = 26}; //"端口选项"
       __property UnicodeString StrGbxModem = { read = fGetMsg, index = 27}; //"调制解调器选项"
       __property UnicodeString StrGbxBuffer= { read = fGetMsg, index = 28}; //"缓存容量"
       __property UnicodeString MsgAAns     = { read = fGetMsg, index = 29}; //"自动应答"
       __property UnicodeString MsgInBuf    = { read = fGetMsg, index = 30}; //"输入缓存"
       __property UnicodeString MsgOutBuf   = { read = fGetMsg, index = 31}; //"输出缓存"
       __property UnicodeString ErrInvAAns  = { read = fGetMsg, index = 32}; //"自动应答必须是 0 - 255 的整数"
       __property UnicodeString ErrInvInBuf = { read = fGetMsg, index = 33}; //"输入缓存容量错误"
       __property UnicodeString ErrInvOutBuf= { read = fGetMsg, index = 34}; //"输出缓存容量错误"
       __property UnicodeString MsgSpacePty = { read = fGetMsg, index = 35}; //"Space Parity"
       __fastcall TMsgStrings();

     private:
       const wchar_t **_Msg;
       UnicodeString __fastcall fGetMsg(int);
       TVictorCommLanguage _CommLanguage;
     };
    __fastcall TForm_YbCommDevice_Settings(TComponent *Owner, TYbCustomCommDevice *lpCommDevice, bool FActive);
private:	// User declarations
    bool __fastcall MoveCtrlFocus(void);
    TYbCustomCommDevice *_YbCommDevice;
    bool  _OldActive, _ForceActive;
    UnicodeString _OldPortName;
    int   _OldByteSize, _OldAutoAAns;
    long  _OldInBufSize, _OldOutBufSize;
    DWORD _OldBaud;
    TYbCommDevice::TParity      _OldParity;
    TYbCommDevice::TStopBits    _OldStopBits;
    TYbCommDevice::TFlowControl _OldFlowControl;
    TCommSerialPortInfo _spi;
};
//---------------------------------------------------------------------------
// extern PACKAGE TForm_YbCommDevice_Settings *Form_YbCommDevice_Settings;
//---------------------------------------------------------------------------
} // namespace VictorComm
} // namespace Vcl
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
