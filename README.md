**rtl-sdr** turns your Realtek RTL2832 based DVB dongle into a SDR receiver

For more information see: http://sdr.osmocom.org/trac/wiki/rtl-sdr

### Additional tuner support

- 06/05/2017 - Added support for MxL5007T tuner

### Build on Windows

CMake files have been modified to compile on Windows using MinGW.

	set PATH=C:\MinGW\bin;C:\Qt\5.8\mingw53_32\bin;C:\Program Files\CMake\bin
    mkdir build
	cd build
	cmake -G "MinGW Makefiles" ../
	mingw32-make

For more information see https://onetransistor.blogspot.com/2017/03/compile-librtlsdr-windows-mingw.html