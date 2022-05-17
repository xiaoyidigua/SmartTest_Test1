//---------------------------------------------------------------------------
//           TVictorEnumSerial.cpp - 获取串行口列表, 版本 1.5.0.6          //
//                           Built: Apr. 24, 2016                          //
//                    Copyright © 1997-2016, Victor Chen                   //
//                     Homepage: http://www.cppfans.com                    //
//                         Email: victor@cppfans.com                       //
//---------------------------------------------------------------------------
#pragma hdrstop
#include "Vcl.VictorEnumSerial.h"
#include <System.Win.Registry.hpp>
#include <setupapi.h>
#include <devguid.h>
#if defined(__clang__) && ((!defined(__POINTER_WIDTH__)) || (__POINTER_WIDTH__==64))
#  pragma link "setupapi.a"
#else
#  pragma link "setupapi.lib"
#endif
#pragma package(smart_init)
//---------------------------------------------------------------------------
namespace Vcl {
namespace VictorComm {
//---------------------------------------------------------------------------
DEFINE_GUID(GUID_CLASS_COMPORT,0x86e0d1e0L,0x8089,0x11d0,0x9c,0xe4,0x08,0x00,0x3e,0x30,0x1f,0x73);
//---------------------------------------------------------------------------
/////////////////////////// TVictorSerialPortInfo ///////////////////////////
//---------------------------------------------------------------------------
__fastcall TVictorSerialPortInfo::TVictorSerialPortInfo()
{
}
//---------------------------------------------------------------------------
__fastcall TVictorSerialPortInfo::TVictorSerialPortInfo(const TVictorSerialPortInfo *lpSPInfo)
{
  Assign(lpSPInfo);
}
//---------------------------------------------------------------------------
__fastcall TVictorSerialPortInfo::~TVictorSerialPortInfo()
{
}
//---------------------------------------------------------------------------
void __fastcall TVictorSerialPortInfo::Assign(const TVictorSerialPortInfo *lpSPInfo)
{
  PortName     = lpSPInfo->PortName    ; // 端口名称
  ClassGuid    = lpSPInfo->ClassGuid   ; // 类型GUID
  ClassDesc    = lpSPInfo->ClassDesc   ; // 类型描述
  InstanceID   = lpSPInfo->InstanceID  ; // 设备标识
  DeviceDesc   = lpSPInfo->DeviceDesc  ; // 设备描述
  FriendlyName = lpSPInfo->FriendlyName; // 设备别名
  LocationInfo = lpSPInfo->LocationInfo; // 位置信息
  RegistryPath = lpSPInfo->RegistryPath; // 注册表的子路径
}
//---------------------------------------------------------------------------
/////////////////////////// TVictorSerialPortList ///////////////////////////
//---------------------------------------------------------------------------
__fastcall TVictorSerialPortList::TVictorSerialPortList()
{
  _SPList = new System::Contnrs::TObjectList;
}
//---------------------------------------------------------------------------
__fastcall TVictorSerialPortList::~TVictorSerialPortList()
{
  delete _SPList;
}
//---------------------------------------------------------------------------
void __fastcall TVictorSerialPortList::EnumPorts(void)
{
  _SPList->Clear();
  _FromRegistry();
  _FromSetupAPI();
  _SPList->Sort(_SortCompare);
}
//---------------------------------------------------------------------------
TVictorSerialPortInfo *__fastcall TVictorSerialPortList::PortByName(const UnicodeString &sPortName)
{
  int n = Count;
  for(int i=0; i<n; i++)
   {
     TVictorSerialPortInfo *lpSPInfo = Ports[i];
     if(lpSPInfo->PortName.CompareIC(sPortName)==0)
       return lpSPInfo;
   }
  return NULL;
}
//---------------------------------------------------------------------------
TVictorSerialPortInfo *__fastcall TVictorSerialPortList::_FGetSPInfo(int idx)
{
  return (TVictorSerialPortInfo*)(*_SPList)[idx];
}
//---------------------------------------------------------------------------
String __fastcall TVictorSerialPortList::_FGetPortName(int idx)
{
  return Ports[idx]->PortName;
}
//---------------------------------------------------------------------------
int __fastcall TVictorSerialPortList::_FGetSPCount(void)
{
  return _SPList->Count;
}
//---------------------------------------------------------------------------
void __fastcall TVictorSerialPortList::_FromRegistry(void)
{
  TRegistry *reg = new TRegistry;
  reg->RootKey = HKEY_LOCAL_MACHINE;
  reg->Access = KEY_READ;
  reg->OpenKey(_T("HARDWARE\\DEVICEMAP\\SERIALCOMM"),false);

  TStringList *slNames = new TStringList;
  reg->GetValueNames(slNames);

  int iPortCount = slNames->Count;
  for(int iPortIdx=0; iPortIdx<iPortCount; iPortIdx++)
   {
     TVictorSerialPortInfo *lpSPInfo = new TVictorSerialPortInfo;
     lpSPInfo->PortName = reg->ReadString((*slNames)[iPortIdx]);
     _SPList->Add(lpSPInfo);
   }

  delete slNames;
  delete reg;
}
//---------------------------------------------------------------------------
void __fastcall TVictorSerialPortList::_FromSetupAPI(void)
{
//HDEVINFO hDevInfo = SetupDiGetClassDevs(NULL, NULL, 0, DIGCF_PRESENT|DIGCF_ALLCLASSES); // 所有设备都读出来了
//HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS, NULL, 0, DIGCF_PRESENT); // 串口和并口列表一起读出来，不易区分
  HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_CLASS_COMPORT, NULL, 0, DIGCF_PRESENT|DIGCF_DEVICEINTERFACE); // 只读取串口列表

  if(hDevInfo!=INVALID_HANDLE_VALUE)
   {
     const int nBufLen = 2048;
     TCHAR szBuf[nBufLen];

     SP_DEVINFO_DATA spDevInfoData;
     memset(&spDevInfoData, 0, sizeof(SP_DEVINFO_DATA));
     spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

     for(DWORD i=0;;i++)
      {
        if(SetupDiEnumDeviceInfo(hDevInfo, i, &spDevInfoData))
         {
           HKEY hDevRegKey = SetupDiOpenDevRegKey(hDevInfo, &spDevInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);
           if(hDevRegKey!=INVALID_HANDLE_VALUE)
            {
              DWORD dwDataType = REG_SZ;
              DWORD dwDataSize = sizeof(TCHAR)*nBufLen;
              if(RegQueryValueEx(hDevRegKey, _T("PortName"), NULL, &dwDataType, (PBYTE)szBuf, &dwDataSize)==ERROR_SUCCESS)
               {
                 TVictorSerialPortInfo *lpSPInfo = PortByName(szBuf);
                 if(!lpSPInfo)
                  {
                    lpSPInfo = new TVictorSerialPortInfo;
                    lpSPInfo->PortName = szBuf;
                    _SPList->Add(lpSPInfo);
                  }
                 lpSPInfo->ClassGuid = GUIDToString(spDevInfoData.ClassGuid);
                 if(SetupDiGetClassDescription(&spDevInfoData.ClassGuid, szBuf, nBufLen, NULL))
                   lpSPInfo->ClassDesc = szBuf;
                 if(SetupDiGetDeviceInstanceId(hDevInfo, &spDevInfoData, szBuf, nBufLen, NULL))
                   lpSPInfo->InstanceID = szBuf;
                 if(SetupDiGetDeviceRegistryProperty(hDevInfo, &spDevInfoData, SPDRP_DEVICEDESC, NULL, (PBYTE)szBuf, sizeof(TCHAR)*nBufLen, NULL))
                   lpSPInfo->DeviceDesc = szBuf;
                 if(SetupDiGetDeviceRegistryProperty(hDevInfo, &spDevInfoData, SPDRP_FRIENDLYNAME, NULL, (PBYTE)szBuf, sizeof(TCHAR)*nBufLen, NULL))
                   lpSPInfo->FriendlyName = szBuf;
                 if(SetupDiGetDeviceRegistryProperty(hDevInfo, &spDevInfoData, SPDRP_DRIVER, NULL, (PBYTE)szBuf, sizeof(TCHAR)*nBufLen, NULL))
                   lpSPInfo->RegistryPath = szBuf;
                 if(SetupDiGetDeviceRegistryProperty(hDevInfo, &spDevInfoData, SPDRP_LOCATION_INFORMATION, NULL, (PBYTE)szBuf, sizeof(TCHAR)*nBufLen, NULL))
                   lpSPInfo->LocationInfo = szBuf;
               }
              RegCloseKey(hDevRegKey);
            }
         }
        else
         {
           DWORD dwErrNo = GetLastError();
           if(dwErrNo==ERROR_NO_MORE_ITEMS)
             break;
         }
      }
     SetupDiDestroyDeviceInfoList(hDevInfo);
   }
}
//---------------------------------------------------------------------------
int __fastcall TVictorSerialPortList::_SortCompare(void *lpItem1, void *lpItem2)
{
  TVictorSerialPortInfo *lpSPInfo1 = (TVictorSerialPortInfo *)lpItem1;
  TVictorSerialPortInfo *lpSPInfo2 = (TVictorSerialPortInfo *)lpItem2;

  int iPortNo1 = TCommSerialPortInfo::PortNo(lpSPInfo1->PortName);
  int iPortNo2 = TCommSerialPortInfo::PortNo(lpSPInfo2->PortName);

  if(iPortNo1>0 && iPortNo2>0)
    return iPortNo1 - iPortNo2; // "COM1" - "COM2"

  if(iPortNo1 != iPortNo2)
    return iPortNo2 - iPortNo1; // "DeviceA" - "COM1"

  return lpSPInfo1->PortName.Compare(lpSPInfo2->PortName);
}
//---------------------------------------------------------------------------
//////////////////////////// TCommSerialPortInfo ////////////////////////////
//---------------------------------------------------------------------------
__fastcall TCommSerialPortInfo::TCommSerialPortInfo()
{
  _PortList = new TVictorSerialPortList;
  _PortList->EnumPorts();
}
//---------------------------------------------------------------------------
__fastcall TCommSerialPortInfo::~TCommSerialPortInfo()
{
  delete _PortList;
}
//---------------------------------------------------------------------------
void __fastcall TCommSerialPortInfo::Refresh(void)
{
  _PortList->EnumPorts();
}
//---------------------------------------------------------------------------
int __fastcall TCommSerialPortInfo::PortNo(UnicodeString s)
{
  int iCOMn = 0;
  if(!s.IsEmpty())
   {
     UnicodeString sPrefix = s.SubString(1,3); //"COM"
     UnicodeString sPortNo = s.SubString(4,s.Length());

     if(sPrefix.UpperCase()==_T("COM"))
      {
        TCHAR *lpEndPtr=NULL;
        iCOMn = wcstol(sPortNo.w_str(),&lpEndPtr,10);
        if(lpEndPtr)if(*lpEndPtr)iCOMn=0; //不符合 "COMn" 格式
        if(iCOMn<0)iCOMn=0; //不符合 "COMn" 格式
      }
   }
  return iCOMn;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall TCommSerialPortInfo::PortName(int iPortNo)
{
  UnicodeString s;
  if(iPortNo>0)
    s.sprintf(_T("COM%d"),iPortNo);
  return s;
}
//---------------------------------------------------------------------------
} // namespace VictorComm
} // namespace Vcl
//---------------------------------------------------------------------------
