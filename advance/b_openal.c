#include "betray.h"

#ifdef BETRAY_OPENAL_AUDIO_WRAPPER

#include <al/al.h>
#include <al/alc.h>

uint betray_audio_sound_create(uint type, uint length, uint frequency, void *data)
{
	uint sound;
	alGenBuffers(1, &sound);
	if(alGetError() != AL_NO_ERROR)
	{
		printf("AL_NO_ERROR");
		return -1;
	}
	alBufferData(sound, AL_FORMAT_MONO16, data, length, frequency);
	printf("Asound %u\n", sound);
	return sound;
}

void betray_audio_sound_destroy(uint sound)
{
	if(sound != -1)
		alDeleteBuffers(1, &sound);
}

uint betray_audio_sound_play(uint sound, float *pos, float *vector, float speed, float volume, boolean loop)
{
	uint source;
	alGenSources(1, &source);
	if(alGetError() != AL_NO_ERROR)
	{
		printf("AL_NO_ERROR A");
		return -1;
	}
	printf("Bsound %u\n", sound);
	alSourcei(source, AL_BUFFER, sound);
	if(alGetError() != AL_NO_ERROR)
	{
		printf("AL_NO_ERROR B");
		return -1;
	}
	alSourcef(source, AL_PITCH, speed);
	alSourcef(source, AL_GAIN, volume);
	if(alGetError() != AL_NO_ERROR)
	{
		printf("AL_NO_ERROR C");
		return -1;
	}
	alSourcefv(source, AL_POSITION, pos);
	if(vector != NULL)
		alSourcefv(source, AL_VELOCITY, vector);
	alSourcei(source, AL_LOOPING, loop);
	if(alGetError() != AL_NO_ERROR)
	{
		printf("AL_NO_ERROR D");
		return -1;
	}
	return source;
}

void betray_audio_sound_set(uint source, float *pos, float *vector, float speed, float volume, boolean loop)
{
	if(source == -1)
	{
		printf("AL_NO_ERROR");
		return -1;
	}
	alSourcef(source, AL_PITCH, speed);
	alSourcef(source, AL_GAIN, volume);
	alSourcefv(source, AL_POSITION, pos);
	alSourcefv(source, AL_VELOCITY, vector);
	alSourcei(source, AL_LOOPING, loop);
}

void betray_audio_sound_stop(uint source)
{
	if(source == -1)
	{
		printf("AL_NO_ERROR");
		return -1;
	}
	alSourceStop(source);
}

void betray_audio_listener(float *pos, float *vector, float *forward, float *side, float scale)
{
	float orientation[6];
	alListenerfv(AL_POSITION, pos);
	alListenerfv(AL_VELOCITY, vector);
	orientation[0] = forward[0]; 
	orientation[1] = forward[1];
	orientation[2] = forward[2];
	orientation[3] = side[0]; 
	orientation[4] = side[1];
	orientation[5] = side[2];
	alListenerfv(AL_ORIENTATION, orientation);
}

void betray_audio_master_volume_set(float volume)
{

}

void betray_audio_init()
{
	alcMakeContextCurrent(alcCreateContext(alcOpenDevice(NULL), NULL));
}

#endif