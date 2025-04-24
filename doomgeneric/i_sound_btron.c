#include <string.h>

#include "doomtype.h"

#include "deh_str.h"
#include "i_sound.h"
#include "m_misc.h"
#include "w_wad.h"
#include "z_zone.h"

#include <btron/event.h>
#include <btron/proctask.h>

// Sound sample rate to use for digital output (Hz)

int snd_samplerate = 44100;

// Maximum number of bytes to dedicate to allocated sound effects.
// (Default: 64MB)

int snd_cachesize = 64 * 1024 * 1024;

// Config variable that controls the sound buffer size.
// We default to 28ms (1000 / 35fps = 1 buffer per tic).

int snd_maxslicetime_ms = 28;

// External command to invoke to play back music.

char *snd_musiccmd = "";

// Low-level sound and music modules we are using

static sound_module_t *sound_module = NULL;
static music_module_t *music_module = NULL;

int snd_musicdevice = SNDDEVICE_SB;
int snd_sfxdevice = SNDDEVICE_SB;

// DOS-specific options: These are unused but should be maintained
// so that the config file can be shared between chocolate
// doom and doom.exe

static int snd_sbport = 0;
static int snd_sbirq = 0;
static int snd_sbdma = 0;
static int snd_mport = 0;

// Compiled-in sound modules:

static sound_module_t *sound_modules[] = 
{
    #ifdef FEATURE_SOUND
    &DG_sound_module,
    #endif
    NULL,
};

static boolean beep_initialized = false;

static uint8_t *current_sound_lump = NULL;
static uint8_t *current_sound_pos = NULL;
static unsigned int current_sound_remaining = 0;
static int current_sound_handle = 0;
static int current_sound_lump_num = -1;

int task_loop = 0;

int frequencies[] = {
       0,  175,  180,  185,  190,  196,  202,  208,  214,  220,
     226,  233,  240,  247,  254,  262,  269,  277,  285,
     294,  302,  311,  320,  330,  339,  349,  359,  370,
     381,  392,  403,  415,  427,  440,  453,  466,  480,
     494,  508,  523,  539,  554,  571,  587,  604,  622,
     640,  659,  679,  698,  719,  740,  762,  784,  807,
     831,  855,  880,  906,  932,  960,  988, 1017, 1046,
    1077, 1109, 1141, 1175, 1210, 1244, 1281, 1318,
    1357, 1397, 1439, 1480, 1523, 1569, 1614, 1661,
    1711, 1762, 1813, 1864, 1921, 1975, 2036, 2093,
    2157, 2217, 2285, 2353, 2420, 2490, 2565, 2639,
};

static void beep_task(W arg)
{
	int timefreq;

	while(task_loop)
	{
		if (current_sound_lump != NULL && current_sound_remaining > 0)
		{
			if (frequencies[*current_sound_pos] != 0)
			{
				timefreq = 0x70000 + frequencies[*current_sound_pos];
			}
			else
			{
				timefreq = -1;
			}

			++current_sound_pos;
			--current_sound_remaining;
		}
		else
		{
			timefreq = -1;
		}

		sig_buz(timefreq);

		dly_tsk(6);
	}

	ext_tsk();
}

static boolean CacheBeepLump(sfxinfo_t *sfxinfo)
{
	int lumplen;
	int headerlen;

	// Free the current sound lump back to the cache

	if (current_sound_lump != NULL)
	{
		W_ReleaseLumpNum(current_sound_lump_num);
		current_sound_lump = NULL;
	}

	// Load from WAD

	current_sound_lump = W_CacheLumpNum(sfxinfo->lumpnum, PU_STATIC);
	lumplen = W_LumpLength(sfxinfo->lumpnum);

	// Read header
  
	if (current_sound_lump[0] != 0x00 || current_sound_lump[1] != 0x00)
	{
		return false;
	}

	headerlen = (current_sound_lump[3] << 8) | current_sound_lump[2];

	if (headerlen > lumplen - 4)
	{
		return false;
	}

	// Header checks out ok

	current_sound_remaining = headerlen;
	current_sound_pos = current_sound_lump + 4;
	current_sound_lump_num = sfxinfo->lumpnum;

	return true;
}

// These Doom PC speaker sounds are not played - this can be seen in the 
// Heretic source code, where there are remnants of this left over
// from Doom.

static boolean IsDisabledSound(sfxinfo_t *sfxinfo)
{
	int i;
	const char *disabled_sounds[] = {
		"posact",
		"bgact",
		"dmact",
		"dmpain",
		"popain",
		"sawidl",
		"rifle",
	};

	for (i=0; i<arrlen(disabled_sounds); ++i)
	{
        	if (!strcmp(sfxinfo->name, disabled_sounds[i]))
		{
			return true;
		}
	}

	return false;
}


void I_InitSound(boolean use_sfx_prefix)
{
	boolean nosound, nosfx, nomusic;

	nosound = M_CheckParm("-nosound") > 0;
	nosfx = M_CheckParm("-nosfx") > 0;

	if(use_sfx_prefix)
	{
		if(!nosound || !nosfx)
		{
			beep_initialized = true;
			task_loop = 1;
			cre_tsk(beep_task, 192, 0);
		}
	}
}

void I_ShutdownSound(void)
{
	task_loop = 0;
}

int I_GetSfxLumpNum(sfxinfo_t *sfxinfo)
{
	char namebuf[9];

	M_snprintf(namebuf, sizeof(namebuf), "dp%s", DEH_String(sfxinfo->name));

	return W_GetNumForName(namebuf);
}

void I_UpdateSound(void)
{

}

void I_UpdateSoundParams(int channel, int vol, int sep)
{

}

int I_StartSound(sfxinfo_t *sfxinfo, int channel, int vol, int sep)
{
	int result;

	if (!beep_initialized)
	{
		return -1;
	}

	if (IsDisabledSound(sfxinfo))
	{
		return -1;
	}

	result = CacheBeepLump(sfxinfo);

	if (result)
	{
		current_sound_handle = channel;
	}

	if (result)
	{
		return channel;
	}
	else
	{
		return -1;
	}
}

void I_StopSound(int channel)
{
	if (!beep_initialized)
	{
        	return;
	}

	// If this is the channel currently playing, immediately end it.

	if (current_sound_handle == channel)
	{
		current_sound_remaining = 0;
	}
}

boolean I_SoundIsPlaying(int channel)
{
	if (!beep_initialized)
	{
		return false;
	}

	if (channel != current_sound_handle)
	{
		return false;
	}

	return current_sound_lump != NULL && current_sound_remaining > 0;
}

void I_PrecacheSounds(sfxinfo_t *sounds, int num_sounds)
{

}

void I_InitMusic(void)
{

}

void I_ShutdownMusic(void)
{

}

void I_SetMusicVolume(int volume)
{

}

void I_PauseSong(void)
{

}

void I_ResumeSong(void)
{

}

void *I_RegisterSong(void *data, int len)
{
	return NULL;
}

void I_UnRegisterSong(void *handle)
{

}

void I_PlaySong(void *handle, boolean looping)
{

}

void I_StopSong(void)
{

}

boolean I_MusicIsPlaying(void)
{
	return false;
}


void I_BindSoundVariables(void)
{
    extern int use_libsamplerate;
    extern float libsamplerate_scale;

    M_BindVariable("snd_musicdevice",   &snd_musicdevice);
    M_BindVariable("snd_sfxdevice",     &snd_sfxdevice);
    M_BindVariable("snd_sbport",        &snd_sbport);
    M_BindVariable("snd_sbirq",         &snd_sbirq);
    M_BindVariable("snd_sbdma",         &snd_sbdma);
    M_BindVariable("snd_mport",         &snd_mport);
    M_BindVariable("snd_maxslicetime_ms", &snd_maxslicetime_ms);
    M_BindVariable("snd_musiccmd",      &snd_musiccmd);
    M_BindVariable("snd_samplerate",    &snd_samplerate);
    M_BindVariable("snd_cachesize",     &snd_cachesize);

#ifdef FEATURE_SOUND
    M_BindVariable("use_libsamplerate",   &use_libsamplerate);
    M_BindVariable("libsamplerate_scale", &libsamplerate_scale);
#endif

    // Before SDL_mixer version 1.2.11, MIDI music caused the game
    // to crash when it looped.  If this is an old SDL_mixer version,
    // disable MIDI.
}

