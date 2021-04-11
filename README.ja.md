- 英語版は [README.md](README.md) です。
- このファイルは開発者向けです。エンドユーザ向けのファイルは readme.ja.html です。

# Ut Video Codec Suite - 高速なロスレス映像コーデック

**Ut Video Codec Suite** は各種のインターフェースをサポートする高速なロスレス映像コーデックです。

以下のインターフェースをサポートします。

- Windows
  - VCM (Video Compression Manager)
  - DMO (DirectX Media Object)


## ビルドに必要な環境

### Windows の場合

- Windows 10 x64
- Visual Studio 2019
- [vcpkg](https://github.com/microsoft/vcpkg) （Visual Studio に integrate されていること）
  - lz4:x86-windows-static
  - lz4:x64-windows-static
  - 以下のパッケージはテストコードのビルドに必要です
    - boost-test:x86-windows
    - boost-test:x64-windows
    - ffmpeg:x86-windows
    - ffmpeg:x64-windows
- [Inno Setup](http://www.jrsoftware.org/isinfo.php)


## 著作権表示とライセンス

Copyright &copy; 2008-2021 梅澤 威志

**Ut Video Codec Suite** は [GNU 一般公衆利用許諾契約書 (GNU GPL) バージョン 2](gplv2.txt) （[日本語参考訳](gplv2.ja.utf8.txt)） またはそれ以降の下でライセンスされます。 詳細は [LICENSE](LICENSE) を参照してください。


## 作者

梅澤 威志 （うめざわ たけし）

- http://umezawa.dyndns.info/wordpress/
- https://github.com/umezawatakeshi
