//---------------------------------------------------------------------------
#pragma hdrstop

#include "Vcl.VictorFSM.h"
#include "Vcl.YbCommDevice.h"
#include "Vcl.YbCommDesignEditors.h"
#include "Vcl.UnitYBCommDeviceAbout.h"
#include "Vcl.UnitYbCommDeviceSettings.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
namespace Vcl {
namespace Ybcommdesigneditors {
//---------------------------------------------------------------------------
void __fastcall TYbCommDeviceEditor::ExecuteVerb(int Index)
{
  if(Index == 0)
   {
     bool bPressedBtnOK;
     VictorComm::TForm_YbCommDevice_Settings *FormSettings;
     VictorComm::TYbCommDevice *lpComm = dynamic_cast<VictorComm::TYbCommDevice*>(Component);

     if(lpComm)
      {
        FormSettings = new VictorComm::TForm_YbCommDevice_Settings(NULL,lpComm,false);
        bPressedBtnOK = FormSettings->ShowModal() == IDOK;
        delete FormSettings;

        if(bPressedBtnOK)
         {
           Designer->Modified();
         }
      }
   }
  else
   {
     VictorComm::TForm_YBComm64_About *FormAbout = new VictorComm::TForm_YBComm64_About(NULL,Component->ClassName());
     FormAbout->ShowModal();
     delete FormAbout;
   }
}
//---------------------------------------------------------------------------
UnicodeString __fastcall TYbCommDeviceEditor::GetVerb(int Index)
{
  if(Index == 0)
    return VictorComm::TForm_YBComm64_About::TMsgStrings().Str_Settings;
  return VictorComm::TForm_YBComm64_About::TMsgStrings().Str_MenuAbout;
}
//---------------------------------------------------------------------------
int __fastcall TYbCommDeviceEditor::GetVerbCount(void)
{
  return 2;
}
//---------------------------------------------------------------------------
void __fastcall PACKAGE Register()
{
  TComponentClass Classes[] =
   {
     __classid(Vcl::VictorComm::TYbCustomCommDevice),
     __classid(Vcl::VictorComm::TYbCommDevice      ),
     __classid(Vcl::VictorClasses::TVictorCustomFSM),
     __classid(Vcl::VictorClasses::TVictorFSM      ),
   };

  TComponentClass Components[] =
   {
     __classid(Vcl::VictorComm::TYbCommDevice      ),
     __classid(Vcl::VictorClasses::TVictorFSM      ),
   };

  ActivateClassGroup(__classid(Vcl::Controls::TControl));

  GroupDescendentsWith(__classid(Vcl::VictorComm::TYbCommDevice      ), __classid(Vcl::Controls::TControl));
  GroupDescendentsWith(__classid(Vcl::VictorComm::TYbCustomCommDevice), __classid(Vcl::Controls::TControl));
  GroupDescendentsWith(__classid(Vcl::VictorClasses::TVictorFSM      ), __classid(Vcl::Controls::TControl));
  GroupDescendentsWith(__classid(Vcl::VictorClasses::TVictorCustomFSM), __classid(Vcl::Controls::TControl));

  RegisterClasses(Classes, sizeof(Classes)/sizeof(Classes[0])-1);

  RegisterComponents("Victor", Components, sizeof(Components)/sizeof(Components[0])-1);
  RegisterComponentEditor(__classid(Vcl::VictorComm::TYbCommDevice), __classid(TYbCommDeviceEditor));
}
//---------------------------------------------------------------------------
} // namespace Ybcommdesigneditors
} // namespace Vcl
//---------------------------------------------------------------------------
