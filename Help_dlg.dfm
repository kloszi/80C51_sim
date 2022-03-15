object Help_form: THelp_form
  Left = 389
  Top = 265
  Width = 422
  Height = 329
  Caption = 'Pomoc'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 127
    Height = 20
    Cursor = crHandPoint
    Caption = 'www.kloszi.prv.pl'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlue
    Font.Height = -16
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    OnClick = Label1Click
  end
  object Label2: TLabel
    Left = 40
    Top = 200
    Width = 75
    Height = 13
    Caption = 'wersja: v  0.122'
  end
  object Memo1: TMemo
    Left = 8
    Top = 32
    Width = 377
    Height = 129
    HideSelection = False
    Lines.Strings = (
      'Czesc '
      
        'Obecnie poszukuje pracy w zawodzie: elektronika ,informatyka (c+' +
        '+,SQL,...)'
      'Wi'#281'cej informacji na mojej stronce www.kloszi.prv.pl'
      ''
      'mail: dioda.fm@interia.pl')
    ReadOnly = True
    TabOrder = 0
  end
  object Button1: TButton
    Left = 168
    Top = 264
    Width = 75
    Height = 25
    Caption = 'OK'
    ModalResult = 1
    TabOrder = 1
  end
end
