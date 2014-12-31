/**
 * @file utauvbdllsample.cpp
 */

#include <iomanip>
#include <fstream>
#include <string>
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <windows.h>


static void
putLog(
  const char *format,
  ...)
{
 char buf[256];
 va_list ap;
 va_start(ap, format);
 std::vsnprintf(buf, sizeof(buf), format, ap);
 va_end(ap);

 std::ofstream ofs("log.txt", std::ios::out | std::ios::app);
 std::time_t t = std::time(nullptr);
 ofs << std::put_time(std::localtime(&t), "%Y-%m-%dT%H:%M:%S");
 ofs << '\t' << buf << std::endl;
}


extern "C" {

/**
 * Call per library loaded.
 *
 * But, loading library per get voice data.
 *
 + @param path Voice bank path.
 */
void _stdcall
load(
  const char* path)
{
 putLog("load(path=\"%s\")", path);
}


/**
 * Call per library unloaded.
 *
 * But, loading library per get voice data.
 * Don't implement heavy process in this func.
 */
void _stdcall
unload()
{
 putLog("unload()");
}


/**
 * Setup vicebank.
 * @param hwnd Prarent window handle? Always 0.
 */
void _stdcall
setup(
  int hwnd)
{
 putLog("setup(hwnd=%X)", hwnd);
 HWND parent = (HWND)hwnd;
 MessageBox(parent, "setup() test", "setup", MB_OK);
}


/**
 * Check voice data exists.
 *
 * @param name Voice name.
 * @param tone (reserved) Voice tone. 
 * @return ==0: not exists, !=0: exists.
 */
int _stdcall
exists(
  const char* name,
  int tone)
{
 putLog("exists(name=\"%s\", tone=%d)", name, tone);
 std::string r("R");
 std::string n(name);
 return (int)(n != r);
}


/**
 * Get voice PCM data.

 * @param name Voice name.
 * @param tone (reserved) Voice tone. 
 * @return Minimum WAV format data (WAVE header, format chunk and data chunk only).
 */
void*  _stdcall
getpcmdata(
  const char* name,
  int tone)
{
 putLog("getpcmdata(name=\"%s\", tone=%d)", name, tone);

 size_t dataLen = 44100;
 size_t dataSize = dataLen * 2;
 size_t bufSize = 44 + dataSize;
 size_t riffChnkSize = bufSize - 8;
 unsigned char* buf = new unsigned char[bufSize];

 buf[0] = 'R'; buf[1] = 'I'; buf[2] = 'F'; buf[3] = 'F';
 buf[4] = ( riffChnkSize        & 0xFF);
 buf[5] = ((riffChnkSize >>  8) & 0xFF);
 buf[6] = ((riffChnkSize >> 16) & 0xFF);
 buf[7] = ((riffChnkSize >> 24) & 0xFF);
 buf[8] = 'W'; buf[9] = 'A'; buf[10] = 'V'; buf[11] = 'E';

 buf[12] = 'f'; buf[13] = 'm'; buf[14] = 't'; buf[15] = ' ';
 buf[16] = 0x10; buf[17] = 0x00; buf[18] = 0x00; buf[19] = 0x00; // size of fmt chunk
 buf[20] = 0x01; buf[21] = 0x00; // fomat ID (0x0001: linear PCM)
 buf[22] = 0x01; buf[23] = 0x00; // number of channels
 buf[24] = 0x44; buf[25] = 0xAC; buf[26] = 0x00; buf[27] = 0x00; // sample rate
 buf[28] = 0x88; buf[29] = 0x58; buf[30] = 0x01; buf[31] = 0x00; // bytes per secound
 buf[32] = 0x02; buf[33] = 0x00; // block size (bytes)
 buf[34] = 0x10; buf[35] = 0x00; // bits per sample

 buf[36] = 'd'; buf[37] = 'a'; buf[38] = 't'; buf[39] = 'a';
 buf[40] = ( dataSize        & 0xFF); // size of data chunk
 buf[41] = ((dataSize >>  8) & 0xFF);
 buf[42] = ((dataSize >> 16) & 0xFF);
 buf[43] = ((dataSize >> 24) & 0xFF);

 for (size_t i = 0; i < dataLen; ++ i) {
  int v = (int)(0x7FFF * std::sin(3.1415926535 * i / (44100.0 / 440.0 / 2.0)));
  buf[44 + i * 2    ] = ( v       & 0xFF);
  buf[44 + i * 2 + 1] = ((v >> 8) & 0xFF);
 }

 return buf;
}


/**
 * Release voice PCM data.
 *
 * @param p Return value of getpcmdata().
y).
 */
void _stdcall
freepcm(
  void *p)
{
 putLog("freepcm(p=%p)", p);

 unsigned char* buf = (unsigned char*)p;
 delete[] buf;
}


/**
 * Get frequecy table data.
 *
 * @param name Voice name.
 * @param tone (reserved) Voice tone. 
 * @return Content of UTAU's ".frq" file.
 */
void* _stdcall
getfrqdata(
  char* name,
  int tone)
{
 putLog("getfrqdata(name=\"%s\", tone=%d)", name, tone);
 return NULL;
}


/**
 * Release frequency table data. 
 *
 * @param p Return value of getfrqdata().
 */
void _stdcall
freefrq(
  void *p)
{
 putLog("freefrq(p=%p)", p);
}


}

