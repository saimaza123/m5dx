//#include	<windows.h>
#include <stddef.h>

// extern "C" int X68Sound_Start(int samprate=44100, int opmflag=1, int
// adpcmflag=1, 				  int betw=5, int pcmbuf=5, int late=200,
// double rev=1.0); extern "C" int X68Sound_Samprate(int samprate=44100); extern
// "C" void X68Sound_Reset(); extern "C" void X68Sound_Free(); extern "C" void
// X68Sound_BetwInt(void (*proc)()=NULL);

// extern "C" int X68Sound_StartPcm(int samprate=44100, int opmflag=1, int
// adpcmflag=1, int pcmbuf=5); extern "C" int X68Sound_GetPcm(void *buf, int
// len);

// extern "C" unsigned char X68Sound_OpmPeek();
// extern "C" void X68Sound_OpmReg(unsigned char no);
// extern "C" void X68Sound_OpmPoke(unsigned char data);
extern "C" void X68Sound_OpmInt(void (*proc)() = NULL);
// extern "C" int X68Sound_OpmWait(int wait=240);
// extern "C" int X68Sound_OpmClock(int clock=4000000);

// extern "C" unsigned char X68Sound_AdpcmPeek();
extern "C" void X68Sound_AdpcmPoke(unsigned char data);
// extern "C" unsigned char X68Sound_PpiPeek();
extern "C" void X68Sound_PpiPoke(unsigned char data);
// extern "C" void X68Sound_PpiCtrl(unsigned char data);
// extern "C" unsigned char X68Sound_DmaPeek(unsigned char adrs);
// extern "C" void X68Sound_DmaPoke(unsigned char adrs, unsigned char data);
// extern "C" void X68Sound_DmaInt(void (*proc)()=NULL);
// extern "C" void X68Sound_DmaErrInt(void (*proc)()=NULL);
// extern "C" void X68Sound_MemReadFunc(int ( *func)(unsigned char *)=NULL);

// extern "C" void X68Sound_WaveFunc(int (*func)()=NULL);

extern "C" int X68Sound_Pcm8_Out(int ch, void* adrs, int mode, int len);
// extern "C" int X68Sound_Pcm8_Aot(int ch, void *tbl, int mode, int cnt);
// extern "C" int X68Sound_Pcm8_Lot(int ch, void *tbl, int mode);
// extern "C" int X68Sound_Pcm8_SetMode(int ch, int mode);
// extern "C" int X68Sound_Pcm8_GetRest(int ch);
// extern "C" int X68Sound_Pcm8_GetMode(int ch);
extern "C" int X68Sound_Pcm8_Abort();

// extern "C" int X68Sound_TotalVolume(int v);

// extern "C" int X68Sound_ErrorCode();
// extern "C" int X68Sound_DebugValue();

// extern int X68Sound_Load();

/*
#define	X68SNDERR_PCMOUT	(-1)
#define	X68SNDERR_TIMER		(-2)
#define	X68SNDERR_MEMORY	(-3)
#define X68SNDERR_NOTACTIVE	(-4)
#define	X68SNDERR_ALREADYACTIVE	(-5)
#define	X68SNDERR_BADARG	(-6)

#define X68SNDERR_DLL		(-1)
#define X68SNDERR_FUNC		(-2)
*/

/*
// DMAに16bit値をビッグエンディアン(68オーダー)で書き込む
#define	X68Sound_DmaPokeW(adrs,data) (X68Sound_DmaPoke((adrs),(unsigned
short)(data)>>8), \ X68Sound_DmaPoke((adrs)+1, (unsigned short)(data)))
// DMAに32bit値をビッグエンディアン(68オーダー)で書き込む
#define	X68Sound_DmaPokeL(adrs,data) (X68Sound_DmaPoke((adrs),(unsigned
int)(data)>>24), \
                                                                                 X68Sound_DmaPoke((adrs)+1, (unsigned int)(data)>>16), \
                                                                                 X68Sound_DmaPoke((adrs)+2, (unsigned int)(data)>>8), \
                                                                                 X68Sound_DmaPoke((adrs)+3, (unsigned int)(data)))
*/
//#define X68Sound_y(no,data)	(X68Sound_OpmReg(no),X68Sound_OpmPoke(data))
