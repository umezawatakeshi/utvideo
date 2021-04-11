- Japanese version is [README.ja.md](README.ja.md).
- This file is for developers. readme.en.html is for end users.

# Ut Video Codec Suite - fast lossless video codec.

**Ut Video Codec Suite** is a fast lossless video codec which supports multiple codec interfaces.

The following interfaces are supported.

- Windows
  - VCM (Video Compression Manager)
  - DMO (DirectX Media Object)


## Build Requirements

### Windows

- Windows 10 x64
- Visual Studio 2019
- [vcpkg](https://github.com/microsoft/vcpkg) (integrated with Visual Studio)
  - lz4:x86-windows-static
  - lz4:x64-windows-static
  - The following packages are required to build test code
    - boost-test:x86-windows
    - boost-test:x64-windows
    - ffmpeg:x86-windows
    - ffmpeg:x64-windows
- [Inno Setup](http://www.jrsoftware.org/isinfo.php)


## Copyright Notice and License

Copyright &copy; 2008-2021 UMEZAWA Takeshi

**Ut Video Codec Suite** is licensed under [GNU General Public License (GNU GPL) version 2](gplv2.txt) or later. See [LICENSE](LICENSE) for more information.


## Author

UMEZAWA Takeshi

- http://umezawa.dyndns.info/wordpress/
- https://github.com/umezawatakeshi
