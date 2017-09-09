#ifndef __MLGUARD_MCNG_MUSICID_H
#define __MLGUARD_MCNG_MUSICID_H

#define MUSIC_ID_MAXLEN 64

struct music_id_track
{
	char title[MUSIC_ID_MAXLEN];
};

struct music_id_medium
{
	char artist[MUSIC_ID_MAXLEN];
	char title[MUSIC_ID_MAXLEN];
	unsigned int track_count;
	struct music_id_track *tracks;
};

#endif
