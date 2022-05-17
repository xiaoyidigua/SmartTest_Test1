/***************************************************************************\
*                                                                           *
*        Unit_YBCommDevice_About.h - 串行口通讯关于窗口, 版本 1.5.0.5       *
*                             Built: Jul. 10, 2015                          *
*                     Copyright © 1997-2015, Victor Chen                    *
*                      Homepage: http://www.cppfans.com                     *
*                          Email: victor@cppfans.com                        *
*                                                                           *
\***************************************************************************/

#ifndef Vcl_UnitYBCommDeviceAboutH
#define Vcl_UnitYBCommDeviceAboutH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
namespace Vcl {
namespace VictorComm {
//---------------------------------------------------------------------------
class PACKAGE TForm_YBComm64_About : public TForm
{
__published:	// IDE-managed Components
    TBitBtn *BnOK;
    TImage *Image1;
    TLabel *LabelCaption;
    TLabel *LabelVersion;
    TLabel *LabelCo;
    TLabel *LabelHomepage;
    TLabel *LabelEmail;
    TBevel *Bevel1;
    TLabel *LabelOSVer;
    void __fastcall LabelHomepageClick(TObject *Sender);
    void __fastcall LabelEmailClick(TObject *Sender);
    void __fastcall BnOKClick(TObject *Sender);
    void __fastcall LabelUrlMouseEnter(TObject *Sender);
    void __fastcall LabelUrlMouseLeave(TObject *Sender);
private:	// User declarations
    void __fastcall ShowSysInfo(void);
public:		// User declarations
    class PACKAGE TMsgStrings
     {
     public:
       __property UnicodeString Str_About        = { read = fGetMsg, index =  0 }; //"关于"
       __property UnicodeString Str_DotDotDot    = { read = fGetMsg, index =  1 }; //"……"
       __property UnicodeString Str_User         = { read = fGetMsg, index =  2 }; //"用户"
       __property UnicodeString Str_Orgn         = { read = fGetMsg, index =  3 }; //"单位"
       __property UnicodeString Str_CRgtVersion  = { read = fGetMsg, index =  4 }; //"版本: 1.00"
       __property UnicodeString Str_CRgtCoName   = { read = fGetMsg, index =  5 }; //"Copyright (c) Victor Chen"
       __property UnicodeString Str_CRgtHomePage = { read = fGetMsg, index =  6 }; //"http://www.cppfans.com"
       __property UnicodeString Str_CRgtAuEmail  = { read = fGetMsg, index =  7 }; //"victor@cppfans.com"
       __property UnicodeString Str_CRgtAuMailTo = { read = fGetMsg, index =  8 }; //"mailto:victor@cppfans.com"
       __property UnicodeString Str_OK           = { read = fGetMsg, index =  9 }; //"确定"
       __property UnicodeString Str_Settings     = { read = fGetMsg, index = 10 }; //"设置(&S)..."
       __property UnicodeString Str_MenuAbout    = { read = fGetMsg, index = 11 }; //"关于(&A)..."
       __fastcall TMsgStrings();

     private:
       const wchar_t **_Msg;
       UnicodeString __fastcall fGetMsg(int);
       TVictorCommLanguage _CommLanguage;
     };
    __fastcall TForm_YBComm64_About(TComponent* Owner, UnicodeString PName);
};
//---------------------------------------------------------------------------
// extern PACKAGE TForm_YBComm64_About *Form_YBComm64_About;
//---------------------------------------------------------------------------
} // namespace VictorComm
} // namespace Vcl
//---------------------------------------------------------------------------
#endif
