object Form_YbCommDevice_Settings: TForm_YbCommDevice_Settings
  Left = 495
  Top = 429
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Form_YbCommDevice_Settings'
  ClientHeight = 260
  ClientWidth = 414
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  PixelsPerInch = 96
  TextHeight = 13
  object GboxBase: TGroupBox
    Left = 8
    Top = 8
    Width = 398
    Height = 121
    Caption = 'GboxBase'
    TabOrder = 0
    object lbBaud: TLabel
      Left = 8
      Top = 59
      Width = 32
      Height = 13
      Caption = 'lbBaud'
    end
    object lbCheck: TLabel
      Left = 8
      Top = 91
      Width = 37
      Height = 13
      Caption = 'lbCheck'
    end
    object lbBits: TLabel
      Left = 213
      Top = 59
      Width = 25
      Height = 13
      Caption = 'lbBits'
    end
    object lbStops: TLabel
      Left = 213
      Top = 91
      Width = 35
      Height = 13
      Caption = 'lbStops'
    end
    object lbPort: TLabel
      Left = 8
      Top = 27
      Width = 28
      Height = 13
      Caption = 'lbPort'
    end
    object CbPort: TComboBox
      Left = 64
      Top = 24
      Width = 322
      Height = 22
      Style = csOwnerDrawFixed
      TabOrder = 0
      OnDrawItem = CbPortDrawItem
      Items.Strings = (
        'COM1'
        'COM2'
        'COM3'
        'COM4')
    end
    object CbSelBaud: TComboBox
      Left = 64
      Top = 56
      Width = 117
      Height = 21
      TabOrder = 1
      Items.Strings = (
        '110'
        '300'
        '600'
        '1200'
        '2400'
        '4800'
        '9600'
        '14400'
        '19200'
        '38400'
        '56000'
        '57600'
        '115200'
        '128000'
        '256000')
    end
    object CbParity: TComboBox
      Left = 64
      Top = 88
      Width = 117
      Height = 21
      Style = csDropDownList
      TabOrder = 2
      Items.Strings = (
        'No parity'
        'Odd'
        'Even'
        'Mark'
        'Space')
    end
    object CbByteSize: TComboBox
      Left = 269
      Top = 56
      Width = 117
      Height = 21
      Style = csDropDownList
      TabOrder = 3
      Items.Strings = (
        '5'
        '6'
        '7'
        '8')
    end
    object CbStopBits: TComboBox
      Left = 269
      Top = 88
      Width = 117
      Height = 21
      Style = csDropDownList
      TabOrder = 4
      Items.Strings = (
        '1'
        '1.5'
        '2')
    end
  end
  object GBoxModem: TGroupBox
    Left = 8
    Top = 135
    Width = 193
    Height = 89
    Caption = 'GBoxModem'
    TabOrder = 1
    object lbFlow: TLabel
      Left = 8
      Top = 27
      Width = 30
      Height = 13
      Caption = 'lbFlow'
    end
    object lbAAns: TLabel
      Left = 8
      Top = 59
      Width = 33
      Height = 13
      Caption = 'lbAAns'
    end
    object CbFlow: TComboBox
      Left = 80
      Top = 24
      Width = 101
      Height = 21
      Style = csDropDownList
      TabOrder = 0
      Items.Strings = (
        'None'
        'RTS/CTS'
        'Xon/Xoff'
        'RTS/CTS & Xon/Xoff')
    end
    object EditAutoAns: TEdit
      Left = 80
      Top = 56
      Width = 101
      Height = 21
      TabOrder = 1
      Text = 'EditAutoAns'
    end
  end
  object GBoxBuffer: TGroupBox
    Left = 213
    Top = 135
    Width = 193
    Height = 89
    Caption = 'GBoxBuffer'
    TabOrder = 2
    object lbInbs: TLabel
      Left = 8
      Top = 27
      Width = 29
      Height = 13
      Caption = 'lbInbs'
    end
    object lbOutbs: TLabel
      Left = 8
      Top = 59
      Width = 37
      Height = 13
      Caption = 'lbOutbs'
    end
    object EditInBufSize: TEdit
      Left = 80
      Top = 24
      Width = 101
      Height = 21
      TabOrder = 0
      Text = 'EditInBufSize'
    end
    object EditOutBufSize: TEdit
      Left = 80
      Top = 56
      Width = 101
      Height = 21
      TabOrder = 1
      Text = 'EditOutBufSize'
    end
  end
  object BnAbout: TBitBtn
    Left = 8
    Top = 230
    Width = 75
    Height = 25
    Cursor = crHandPoint
    Caption = '&About'
    Glyph.Data = {
      F6000000424DF600000000000000760000002800000010000000100000000100
      0400000000008000000000000000000000001000000000000000000000000000
      8000008000000080800080000000800080008080000080808000C0C0C0000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00DDDDDDDDD007
      DDDDDDDDDDD70F07DDDDDDDDD700FF07DDDDDDDD008FFF077DDDDDD08FFFFF00
      77DDDD78FFFFFFF8077DD7FFFFFFFFFFF07DD7FFFFCCCCFFF0777FFFFFFCCFFF
      FF077FFFFFFCCFFFFF077FFFFFCCCFFFFF077FFFFFFFFFFFFF0DD7FFFFFCCFFF
      F0DDD78FFFF88FFFF0DDDD778FFFFFF77DDDDDDD7777777DDDDD}
    Spacing = 6
    TabOrder = 3
    OnClick = BnAboutClick
  end
  object BnDefault: TBitBtn
    Left = 171
    Top = 230
    Width = 75
    Height = 25
    Cursor = crHandPoint
    Caption = '&Default'
    Glyph.Data = {
      F6000000424DF600000000000000760000002800000010000000100000000100
      0400000000008000000000000000000000001000000000000000000000000000
      8000008000000080800080000000800080008080000080808000C0C0C0000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333334444433
      333324334222224433332244222222224333222222AAAA22243322222A3333A2
      224322222333333A224322222233333A4443AAAAAAA333333333333333333344
      4444A44433333A222224A224333333A222243A224333344222243A2224444222
      222433A222222222AA24333AA22222AA33A333333AAAAA333333}
    Spacing = 6
    TabOrder = 4
    OnClick = BnDefaultClick
  end
  object BnOK: TBitBtn
    Left = 251
    Top = 230
    Width = 75
    Height = 25
    Cursor = crHandPoint
    Caption = '&OK'
    Default = True
    Glyph.Data = {
      F6000000424DF600000000000000760000002800000010000000100000000100
      0400000000008000000000000000000000001000000000000000000000000000
      8000008000000080800080000000800080008080000080808000C0C0C0000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
      3333333344333333333333342243333333333342222433333333342222224333
      33334222A22224333333222A3A2224333333A2A333A2224333333A33333A2224
      333333333333A2224333333333333A2224333333333333A2224333333333333A
      2224333333333333A2243333333333333A2233333333333333A3}
    Spacing = 6
    TabOrder = 5
    OnClick = BnOKClick
  end
  object BnCancel: TBitBtn
    Left = 331
    Top = 230
    Width = 75
    Height = 25
    Cursor = crHandPoint
    Cancel = True
    Caption = '&Cancel'
    Glyph.Data = {
      F6000000424DF600000000000000760000002800000010000000100000000100
      04000000000080000000CE0E0000C40E00001000000000000000000000000000
      8000008000000080800080000000800080008080000080808000C0C0C0000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00887788888888
      8888891178888897888889111788891178888911117891111788889111171111
      1788888911111111788888889111111788888888811111788888888889111178
      8888888891111178888888891117111788888891117891117888889117888911
      1788888918888891117888888888888919888888888888888888}
    Spacing = 6
    TabOrder = 6
    OnClick = BnCancelClick
  end
end
