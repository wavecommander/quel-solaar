#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <math.h>

typedef unsigned int uint;

#define BETRAY_AUDIO_BLOCK_COUNT 8
#define BETRAY_AUDIO_BLOCK_BYTE_SIZE 4000

static HWAVEOUT	b_audio_win_device;
static uint		b_audio_win_next_free = 0;
static uint		b_audio_win_blocks_free = BETRAY_AUDIO_BLOCK_COUNT;

static CRITICAL_SECTION b_audio_win_mutex;


static WAVEHDR		b_audio_win_headers[BETRAY_AUDIO_BLOCK_COUNT];
static void		*b_audio_win_blocks[BETRAY_AUDIO_BLOCK_COUNT];


uint b_audio_win_pos = 0;

void b_audio_win_update_callback(void *data, uint length, uint padding, float *vec)
{
	static uint position = 0;
	uint i;
	short *buf;
	buf = data;
	for(i = 0; i < length; i++)
		buf[i * padding] = 32000.0 * sin((double)(b_audio_win_pos + i) * 0.3) * 0.1;
}

void b_audio_win_update_start(uint length)
{
	b_audio_win_pos += length;
}

void betray_audio_update_callback(void *data, uint length, uint padding, float *vec);
void betray_audio_time_callback(uint length);

#ifdef BETRAY_WIN32_AUDIO_WRAPPER

static void CALLBACK waveOutProc(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
    int* freeBlockCounter = (int*)dwInstance;
    if(uMsg != WOM_DONE)
		return;
    EnterCriticalSection(&b_audio_win_mutex);
    (*freeBlockCounter)++;
    LeaveCriticalSection(&b_audio_win_mutex);
}

#endif

void b_audio_win_update_sound()
{
#ifdef BETRAY_WIN32_AUDIO_WRAPPER
	float left[3] = {-1, 0, 0};
	float right[3] = {1, 0, 0};
	uint i;
	i = b_audio_win_blocks_free;
    EnterCriticalSection(&b_audio_win_mutex);
    b_audio_win_blocks_free = 0;
    LeaveCriticalSection(&b_audio_win_mutex);
	while(i > 0)
	{		
		if(b_audio_win_headers[b_audio_win_next_free].dwFlags & WHDR_PREPARED) 
			 waveOutUnprepareHeader(b_audio_win_device, &b_audio_win_headers[b_audio_win_next_free], sizeof(WAVEHDR));
		ZeroMemory(&b_audio_win_headers[b_audio_win_next_free], sizeof(WAVEHDR));
		b_audio_win_headers[b_audio_win_next_free].dwBufferLength = BETRAY_AUDIO_BLOCK_BYTE_SIZE;
		b_audio_win_headers[b_audio_win_next_free].lpData = b_audio_win_blocks[b_audio_win_next_free];

		betray_audio_update_callback(&((short *)b_audio_win_blocks[b_audio_win_next_free])[1], BETRAY_AUDIO_BLOCK_BYTE_SIZE / 4, 2, left);
		betray_audio_update_callback(((short *)b_audio_win_blocks[b_audio_win_next_free]), BETRAY_AUDIO_BLOCK_BYTE_SIZE / 4, 2, right);
		betray_audio_time_callback(BETRAY_AUDIO_BLOCK_BYTE_SIZE / 4);
		waveOutPrepareHeader(b_audio_win_device, &b_audio_win_headers[b_audio_win_next_free], sizeof(WAVEHDR));
		waveOutWrite(b_audio_win_device, &b_audio_win_headers[b_audio_win_next_free], sizeof(WAVEHDR));
		b_audio_win_next_free = (b_audio_win_next_free + 1) % BETRAY_AUDIO_BLOCK_COUNT;
		i--;
	}
#endif
}

#ifdef BETRAY_WIN32_AUDIO_WRAPPER

void writeAudioBlock(HWAVEOUT hWaveOut, LPSTR block, DWORD size)
{
    WAVEHDR header;
    /*
    * initialise the block header with the size
    * and pointer.
    */
    ZeroMemory(&header, sizeof(WAVEHDR));
    header.dwBufferLength = size;
    header.lpData = block;
    /*
    * prepare the block for playback
    */
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    /*
    * write the block to the device. waveOutWrite returns immediately
    * unless a synchronous driver is used (not often).
    */
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    /*
    * wait a while for the block to play then start trying
    * to unprepare the header. this will fail until the block has
    * played.
    */
    Sleep(500);
    while(waveOutUnprepareHeader(
    hWaveOut, 
    &header, 
    sizeof(WAVEHDR)
    ) == WAVERR_STILLPLAYING)
    Sleep(100);
}

#endif

void b_audio_win_init()
{
#ifdef BETRAY_WIN32_AUDIO_WRAPPER
    WAVEFORMATEX settings; /* look this up in your documentation */
    MMRESULT result;/* for waveOut return values */
	uint i;
    settings.nSamplesPerSec = 44100; /* sample rate */
    settings.wBitsPerSample = 16; /* sample size */
    settings.nChannels = 2; /* channels*/
    settings.cbSize = 0; /* size of _extra_ info */
    settings.wFormatTag = WAVE_FORMAT_PCM;
    settings.nBlockAlign = (settings.wBitsPerSample >> 3) * settings.nChannels;
    settings.nAvgBytesPerSec = settings.nBlockAlign * settings.nSamplesPerSec;
printf("b_audio_win_init\n");

    if(waveOutOpen(&b_audio_win_device, WAVE_MAPPER, &settings, waveOutProc, &b_audio_win_blocks_free, CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
	{
		sprintf(stderr, "Failed to open audio device\n");
         exit(0);
    }
	InitializeCriticalSection(&b_audio_win_mutex);
	for(i = 0; i < BETRAY_AUDIO_BLOCK_COUNT; i++)
	{
		ZeroMemory(&b_audio_win_headers[i], sizeof(WAVEHDR));
		b_audio_win_blocks[i] = malloc(BETRAY_AUDIO_BLOCK_BYTE_SIZE);
	}
	#endif
}
/*int main(int argc, char* argv[])
{
	b_audio_win_init();

    printf("The Wave Mapper device was opened successfully!\n");
	b_audio_win_update_sound();
	Sleep(50000);
    waveOutClose(b_audio_win_device);
    return 0;
}*/
