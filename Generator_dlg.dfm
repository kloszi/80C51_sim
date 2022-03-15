object Generator: TGenerator
  Left = 366
  Top = 275
  Width = 333
  Height = 371
  Caption = 'Generator'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl1: TPageControl
    Left = 16
    Top = 16
    Width = 305
    Height = 281
    ActivePage = TabSheet1
    TabIndex = 0
    TabOrder = 0
    object TabSheet1: TTabSheet
      Caption = 'Ustawienia'
      object Label1: TLabel
        Left = 8
        Top = 32
        Width = 105
        Height = 20
        Caption = 'Cz'#281'stotliwo'#347#263
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -16
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object Label2: TLabel
        Left = 16
        Top = 80
        Width = 90
        Height = 20
        Caption = 'Wypelnienie'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -16
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object Label4: TLabel
        Left = 192
        Top = 72
        Width = 25
        Height = 24
        Caption = '[%]'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -19
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object CSpinEdit1: TCSpinEdit
        Left = 128
        Top = 24
        Width = 57
        Height = 30
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -16
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        MaxValue = 999
        MinValue = 1
        ParentFont = False
        TabOrder = 0
        Value = 10
      end
      object CSpinEdit2: TCSpinEdit
        Left = 128
        Top = 72
        Width = 57
        Height = 30
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -16
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        MaxValue = 99
        MinValue = 1
        ParentFont = False
        TabOrder = 1
        Value = 50
      end
      object ComboBox1: TComboBox
        Left = 192
        Top = 24
        Width = 57
        Height = 28
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -16
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ItemHeight = 20
        ItemIndex = 0
        ParentFont = False
        TabOrder = 2
        Text = '[Hz]'
        OnChange = ComboBox1Change
        Items.Strings = (
          '[Hz]'
          '[kHz]')
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Pomoc'
      ImageIndex = 1
      object Memo1: TMemo
        Left = 0
        Top = 0
        Width = 297
        Height = 249
        TabOrder = 0
      end
    end
  end
  object OK: TButton
    Left = 112
    Top = 304
    Width = 75
    Height = 25
    Caption = 'OK'
    ModalResult = 1
    TabOrder = 1
    OnClick = OKClick
  end
end
