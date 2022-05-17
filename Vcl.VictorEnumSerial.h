//---------------------------------------------------------------------------
//            TVictorEnumSerial.h - 获取串行口列表, 版本 1.5.0.6           //
//                           Built: Apr. 24, 2016                          //
//                    Copyright © 1997-2016, Victor Chen                   //
//                     Homepage: http://www.cppfans.com                    //
//                         Email: victor@cppfans.com                       //
//---------------------------------------------------------------------------
#ifndef Vcl_VictorEnumSerialH
#define Vcl_VictorEnumSerialH
//---------------------------------------------------------------------------
#include <vcl.h>
#include <tchar.h>
//---------------------------------------------------------------------------
namespace Vcl {
namespace VictorComm {
//---------------------------------------------------------------------------
class PACKAGE TVictorSerialPortInfo : public TObject
{
public:
  String PortName;      // 端口名称
  String ClassGuid;     // 类型GUID
  String ClassDesc;     // 类型描述
  String InstanceID;    // 设备标识
  String DeviceDesc;    // 设备描述
  String FriendlyName;  // 设备别名
  String LocationInfo;  // 位置信息
  String RegistryPath;  // 注册表的子路径

  void __fastcall Assign(const TVictorSerialPortInfo *lpSPInfo);

  __fastcall TVictorSerialPortInfo();
  __fastcall TVictorSerialPortInfo(const TVictorSerialPortInfo *lpSPInfo);
  __fastcall ~TVictorSerialPortInfo();
};
//---------------------------------------------------------------------------
class PACKAGE TVictorSerialPortList : public TObject
{
public:
  __property TVictorSerialPortInfo *Ports    [int] = { read = _FGetSPInfo   };
  __property String                 PortNames[int] = { read = _FGetPortName };
  __property String                 Strings  [int] = { read = _FGetPortName }; // same as PortNames[int], compatible with earlier versions
  __property int                    Count          = { read = _FGetSPCount  };

  virtual void __fastcall EnumPorts(void);
  TVictorSerialPortInfo *__fastcall PortByName(const UnicodeString &sPortName);

  __fastcall TVictorSerialPortList();
  __fastcall ~TVictorSerialPortList();
private:
  System::Contnrs::TObjectList *_SPList;
  TVictorSerialPortInfo *__fastcall _FGetSPInfo(int idx);
  String __fastcall _FGetPortName(int idx);
  int __fastcall _FGetSPCount(void);
  void __fastcall _FromRegistry(void);
  void __fastcall _FromSetupAPI(void);
  static int __fastcall _SortCompare(void *lpItem1, void *lpItem2);
};
//---------------------------------------------------------------------------
class PACKAGE TCommSerialPortInfo // compatible with earlier versions
{
public:
  __property TVictorSerialPortList *PortList = { read = _PortList };
  void __fastcall Refresh(void);

  static int __fastcall PortNo(UnicodeString s);
  static UnicodeString __fastcall PortName(int iPortNo);

  __fastcall TCommSerialPortInfo();
  virtual __fastcall ~TCommSerialPortInfo();

private:
  TVictorSerialPortList *_PortList;
};
//---------------------------------------------------------------------------
} // namespace VictorComm
} // namespace Vcl
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
