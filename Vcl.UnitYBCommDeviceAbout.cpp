/***************************************************************************\
*                                                                           *
*       Unit_YBCommDevice_About.cpp - 串行口通讯关于窗口, 版本 1.5.0.6      *
*                             Built: Jul. 10, 2015                          *
*                     Copyright © 1997-2015, Victor Chen                    *
*                      Homepage: http://www.cppfans.com                     *
*                          Email: victor@cppfans.com                        *
*                                                                           *
\***************************************************************************/

#include <vcl.h>
#pragma hdrstop

#include "Vcl.VictorComm.h"
#include "Vcl.UnitYBCommDeviceAbout.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
namespace Vcl {
namespace VictorComm {
//---------------------------------------------------------------------------
// TForm_YBComm64_About *Form_YBComm64_About;
//---------------------------------------------------------------------------
__fastcall TForm_YBComm64_About::TForm_YBComm64_About(TComponent* Owner, UnicodeString PName)
    : TForm(Owner)
{
  LabelCaption->ParentFont = true;
  LabelCaption->Font->Size = 11;

  LabelHomepage->Font->Color = clBlue;
  LabelHomepage->Font->Style = TFontStyles();

  LabelEmail->Font->Color = clBlue;
  LabelEmail->Font->Style = TFontStyles();

  TMsgStrings Msgs;
  Caption = Msgs.Str_About + PName + Msgs.Str_DotDotDot;
  LabelCaption ->Caption = PName;
  LabelVersion ->Caption = Msgs.Str_CRgtVersion ; //"版本: 1.00"
  LabelCo      ->Caption = Msgs.Str_CRgtCoName  ; //"Copyright (c) Victor Chen"
  LabelHomepage->Caption = Msgs.Str_CRgtHomePage; //"http://www.cppfans.com"
  LabelHomepage->Hint    = Msgs.Str_CRgtHomePage; //"http://www.cppfans.com"
  LabelEmail   ->Caption = Msgs.Str_CRgtAuEmail ; //"victor@cppfans.com"
  LabelEmail   ->Hint    = Msgs.Str_CRgtAuMailTo; //"mailto:victor@cppfans.com"
//LabelPmtUser ->Caption = Msgs.Str_User        ; //"用户"
//LabelPmtOrgn ->Caption = Msgs.Str_Orgn        ; //"单位"
  BnOK         ->Caption = Msgs.Str_OK          ; //"确定"

  ShowSysInfo();
}  
//---------------------------------------------------------------------------
void __fastcall TForm_YBComm64_About::ShowSysInfo(void)
{
  bool bIsNT = Sysutils::Win32Platform == VER_PLATFORM_WIN32_NT;
  UnicodeString sOSName, sCurrentVersion, sCurrentBuildNumber;
  UnicodeString sCSDVersion, sCSDBuildNumber;
  UnicodeString sRegUser, sRegOrg;

  HKEY hKey;
  bool bRegOK = bIsNT ? RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Windows NT\\CurrentVersion"), 0, KEY_READ, &hKey)==ERROR_SUCCESS:
                        RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Windows\\CurrentVersion"), 0, KEY_READ, &hKey)==ERROR_SUCCESS;
  if(bRegOK)
   {
     try
      {
        TCHAR sValue[256];

        DWORD dwValueSize=sizeof(sValue);
        if(RegQueryValueEx(hKey, _T("RegisteredOwner"), NULL, NULL, (BYTE*)sValue, &dwValueSize)==ERROR_SUCCESS)
          sRegUser = sValue;

        dwValueSize=sizeof(sValue);
        if(RegQueryValueEx(hKey, _T("RegisteredOrganization"), NULL, NULL, (BYTE*)sValue, &dwValueSize)==ERROR_SUCCESS)
          sRegOrg = sValue;

        dwValueSize=sizeof(sValue);
        if(RegQueryValueEx(hKey, _T("ProductName"), NULL, NULL, (BYTE*)sValue, &dwValueSize)==ERROR_SUCCESS)
          sOSName = sValue;

        dwValueSize=sizeof(sValue);
        if(RegQueryValueEx(hKey, _T("CurrentVersion"), NULL, NULL, (BYTE*)sValue, &dwValueSize)==ERROR_SUCCESS)
          sCurrentVersion = sValue;

        dwValueSize=sizeof(sValue);
        if(RegQueryValueEx(hKey, _T("CurrentBuildNumber"), NULL, NULL, (BYTE*)sValue, &dwValueSize)==ERROR_SUCCESS)
          sCurrentBuildNumber = sValue;

        dwValueSize=sizeof(sValue);
        if(RegQueryValueEx(hKey, _T("CSDVersion"), NULL, NULL, (BYTE*)sValue, &dwValueSize)==ERROR_SUCCESS)
         {
           sCSDVersion = sValue;
           dwValueSize=sizeof(sValue);
           if(RegQueryValueEx(hKey, _T("CSDBuildNumber"), NULL, NULL, (BYTE*)sValue, &dwValueSize)==ERROR_SUCCESS)
             sCSDBuildNumber = sValue;
           sCSDBuildNumber = sCSDBuildNumber.Trim();
           if(!sCSDBuildNumber.IsEmpty())
             sCSDVersion += _T(" ") + sCSDBuildNumber;
         }
      }
     __finally
      {
        RegCloseKey(hKey);
      }
   }

  if(sOSName.IsEmpty())
    sOSName = _T("Windows");
  sOSName += _T(" ") + sCurrentVersion + _T(".") + sCurrentBuildNumber;
  sCSDVersion = sCSDVersion.Trim();
  if(!sCSDVersion.IsEmpty())
    sOSName += _T(", ") + sCSDVersion;
  LabelOSVer->Caption = sOSName;
//LabelUser->Caption = sRegUser;
//LabelUserOrg->Caption = sRegOrg;
}
//---------------------------------------------------------------------------
void __fastcall TForm_YBComm64_About::LabelUrlMouseEnter(TObject *Sender)
{
  TLabel *lpLabel = dynamic_cast<TLabel *>(Sender);
  if(lpLabel)
   {
     lpLabel->Font->Color = clRed;
     lpLabel->Font->Style = TFontStyles()<<fsUnderline;
   }
}
//---------------------------------------------------------------------------
void __fastcall TForm_YBComm64_About::LabelUrlMouseLeave(TObject *Sender)
{
  TLabel *lpLabel = dynamic_cast<TLabel *>(Sender);
  if(lpLabel)
   {
     lpLabel->Font->Color = clBlue;
     lpLabel->Font->Style = TFontStyles();
   }
}
//---------------------------------------------------------------------------
void __fastcall TForm_YBComm64_About::LabelHomepageClick(TObject *Sender)
{
  ShellExecute(Handle, _T("open"), TMsgStrings().Str_CRgtHomePage.w_str(), NULL, _T(""), SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------
void __fastcall TForm_YBComm64_About::LabelEmailClick(TObject *Sender)
{
  ShellExecute(Handle, _T("open"), TMsgStrings().Str_CRgtAuMailTo.w_str(), NULL, _T(""), SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------
void __fastcall TForm_YBComm64_About::BnOKClick(TObject *Sender)
{
  ModalResult = IDOK;
}
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
__fastcall TForm_YBComm64_About::TMsgStrings::TMsgStrings()
{
  static const wchar_t *_Msg_Chs[] =
   {
     L"关于 "                    , // 0 Str_About
     L"……"                       , // 1 Str_DotDotDot
     L"用户:"                    , // 2 Str_User
     L"单位:"                    , // 3 Str_Orgn
     L"版本: 1.5.0.6"            , // 4 Str_CRgtVersion
     L"Copyright © Victor Chen"  , // 5 Str_CRgtCoName
     L"http://www.cppfans.com"   , // 6 Str_CRgtHomePage
     L"victor@cppfans.com"       , // 7 Str_CRgtAuEmail
     L"mailto:victor@cppfans.com", // 8 Str_CRgtAuMailTo
     L"确定"                     , // 9 Str_OK
     L"设置(&S)..."              , //10 Str_Settings
     L"关于(&A)..."              , //11 Str_MenuAbout
   };
  static const wchar_t *_Msg_Cht[] =
   {
     L"關於 "                    , // 0 Str_About
     L"……"                       , // 1 Str_DotDotDot
     L"用戶:"                    , // 2 Str_User
     L"單位:"                    , // 3 Str_Orgn
     L"版本: 1.5.0.6"            , // 4 Str_CRgtVersion
     L"Copyright © Victor Chen"  , // 5 Str_CRgtCoName
     L"http://www.cppfans.com"   , // 6 Str_CRgtHomePage
     L"victor@cppfans.com"       , // 7 Str_CRgtAuEmail
     L"mailto:victor@cppfans.com", // 8 Str_CRgtAuMailTo
     L"確定"                     , // 9 Str_OK
     L"設置(&S)..."              , //10 Str_Settings
     L"關於(&A)..."              , //11 Str_MenuAbout
   };
  static const wchar_t *_Msg_Enu[] =
   {
     L"About "                   , //0 Str_About
     L"..."                      , //1 Str_DotDotDot
     L"User:"                    , //2 Str_User
     L"Orgn:"                    , //3 Str_Orgn
     L"Version: 1.5.0.6"         , //4 Str_CRgtVersion
     L"Copyright © Victor Chen"  , //5 Str_CRgtCoName
     L"http://www.cppfans.com"   , //6 Str_CRgtHomePage
     L"victor@cppfans.com"       , //7 Str_CRgtAuEmail
     L"mailto:victor@cppfans.com", //8 Str_CRgtAuMailTo
     L"OK"                       , //9 Str_OK
     L"&Settings..."             , //10 Str_Settings
     L"&About..."                , //11 Str_MenuAbout
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
UnicodeString __fastcall TForm_YBComm64_About::TMsgStrings::fGetMsg(int mt)
{
  return _Msg[mt];
}
//---------------------------------------------------------------------------
} // namespace VictorComm
} // namespace Vcl
//---------------------------------------------------------------------------
