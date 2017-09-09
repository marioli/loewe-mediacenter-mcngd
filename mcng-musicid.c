/* 
 * This file is part of mcngd - the Mediacenter Next Generation Daemon
 *   (https://github.com/marioli/loewe-mediacenter-mcngd)
 * Copyright (c) 2017 Mario Lombardo
 * 
 * mcngd is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU Lesser General Public License as   
 * published by the Free Software Foundation, version 3.
 *
 * mcngd is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * mcng-musicid.c:
 * functions to enable mcngd to lookup the ID and covers of a 
 * particular music release or entity.
 * Thanks to Andrew Hawkins as he provided examples for the usage
 * of libmusicbrainz. 
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <discid/discid.h>
#include <musicbrainz5/mb5_c.h>
#include "mcng-musicid.h"

#define TOC "150,12605,28564,43931,56440,74277,91000,108421,124324,138881,151925,171086,188472,202650"

struct music_id_medium *mcngd_calculate_cddb_id(const char *toc)
{
	DiscId *d = NULL;
	Mb5Query mb5q = NULL;
	Mb5Metadata mb5md1 = NULL, mb5md2 = NULL;
	Mb5Disc mb5d = NULL;
	Mb5ReleaseList mb5rl = NULL;
	Mb5Release mb5r = NULL;
	tQueryResult result = 0;
	int http_code = 0;
	char ErrorMessage[256];
	int *toc_info_offset_list = NULL;
	size_t toclen, tval_pos;
	const char *toc_ptr = NULL;
	uint16_t n;
	int parse_error = 0;
	uint8_t toc_info_tracks = 0;
	uint8_t current_track = 0;
	char tmp_val[32];
	struct music_id_medium *identified_disc = NULL;

	if(!toc)
	{
		return NULL;
	}

	/* first we count the tracks. As the TOC has an entry each track plus the total sectors
	 * the amount of comma equals the amount of tracks
	 */
	toclen = strlen(toc);
	toc_ptr = toc;
	for(n = 0; n < toclen; n++)
	{
		if(*toc_ptr == ',')
		{
			toc_info_tracks++;
		}
		toc_ptr++;
	}

	/* we need to add one more entry for the total sector information */
	if(!(toc_info_offset_list = malloc((toc_info_tracks + 1) * sizeof(int))))
	{
		return NULL;
	}

	/* now we copy the sector counts into the int array */
	toc_ptr = toc;
	current_track = 1;
	tval_pos = 0;
	parse_error = 0;
	for(n = 0; n <= toclen; n++)
	{
		if(*toc_ptr >= '0' && *toc_ptr <= '9')
		{
			tmp_val[tval_pos++] = *toc_ptr;
			if(tval_pos >= sizeof(tmp_val))
			{
				parse_error = 1;
				break;
			}
		}
		else if(*toc_ptr == ',' || *toc_ptr == 0)
		{
			tmp_val[tval_pos] = 0;
			toc_info_offset_list[current_track] = strtol(tmp_val, NULL, 10);
			/* printf("toc_info_offset_list on %d is %d\n", current_track, toc_info_offset_list[current_track]); */
			tval_pos = 0;
			current_track++;
			current_track = current_track % (toc_info_tracks + 1);
		}
		else
		{
			parse_error = 1;
			break;
		}
		toc_ptr++;
	}

	if(parse_error)
	{
		free(toc_info_offset_list);
		return NULL;
	}

	/* Query discid by the list of offsets */
	d = discid_new();
	if(1 != discid_put(d, 1, toc_info_tracks, toc_info_offset_list))
	{
		free(toc_info_offset_list);
		return NULL;
	}
	free(toc_info_offset_list);

	printf("musicbrainz disk-id: %s\n", discid_get_id(d));
	printf("cddb        disk-id: %s\n", discid_get_freedb_id(d));

	/* ask libmusicbrainz to create a new query handle */
	if(NULL == (mb5q = mb5_query_new("mcngd-1.0", NULL, 0)))
	{
		discid_free(d);
		return NULL;
	}
	mb5md1 = mb5_query_query(mb5q, "discid", discid_get_id(d), "", 0, NULL, NULL);

	result = mb5_query_get_lastresult(mb5q);
	http_code = mb5_query_get_lasthttpcode(mb5q);

	if(result != 0 || http_code != 200)
	{
		mb5_query_get_lasterrormessage(mb5q, ErrorMessage, sizeof(ErrorMessage));
		printf("result: %d\nHTTPCode: %d\nErrorMessage: '%s'\n", result, http_code, ErrorMessage);
		mb5_metadata_delete(mb5md1);
		mb5md1 = NULL;
	}

	if(!mb5md1)
	{
		mb5_query_delete(mb5q);
		discid_free(d);
		return NULL;
	}

	mb5d = mb5_metadata_get_disc(mb5md1);
	if(mb5d)
	{
		mb5rl = mb5_disc_get_releaselist(mb5d);
		if(mb5rl)
		{
			printf("Found %d release(s)\n",mb5_release_list_size(mb5rl));

			for(n = 0;n < mb5_release_list_size(mb5rl); n++)
			{
				mb5md2 = NULL;
				mb5r = mb5_release_list_item(mb5rl, n);

				if(mb5r)
				{
					/* The releases returned from LookupDiscID don't contain full information */

					char **ParamNames;
					char **ParamValues;
					char ReleaseID[256];

					ParamNames=malloc(sizeof(char *));
					ParamNames[0]=malloc(256);
					ParamValues=malloc(sizeof(char *));
					ParamValues[0]=malloc(256);

					strcpy(ParamNames[0],"inc");
					strcpy(ParamValues[0],"artists labels recordings release-groups url-rels discids artist-credits");

					mb5_release_get_id(mb5r,ReleaseID,sizeof(ReleaseID));

					mb5md2 = mb5_query_query(mb5q,"release",ReleaseID,"",1,ParamNames,ParamValues);

					if (mb5md2)
					{
						Mb5Release FullRelease=mb5_metadata_get_release(mb5md2);
						if (FullRelease)
						{
							/*
							 * However, these releases will include information for all media in the release
							 * So we need to filter out the only the media we want.
							 */

							Mb5MediumList MediumList=mb5_release_media_matching_discid(FullRelease,discid_get_id(d));
							if (MediumList)
							{
								if (mb5_medium_list_size(MediumList))
								{
									int ThisMedium=0;

									Mb5ReleaseGroup ReleaseGroup=mb5_release_get_releasegroup(FullRelease);
									if (ReleaseGroup)
									{
										/* One way of getting a string, just use a buffer that
										 * you're pretty sure will accomodate the whole string
										 */

										char Title[256];

										mb5_releasegroup_get_title(ReleaseGroup,Title,sizeof(Title));
										printf("Release group title: '%s'\n",Title);
									}
									else
										printf("No release group for this release\n");

									printf("Found %d media item(s)\n",mb5_medium_list_size(MediumList));

									for (ThisMedium=0;ThisMedium<mb5_medium_list_size(MediumList);ThisMedium++)
									{
										Mb5Medium Medium=mb5_medium_list_item(MediumList,ThisMedium);
										if (Medium)
										{
											int AllocSize=10;
											char *MediumTitle=malloc(AllocSize);
											int RequiredSize;

											Mb5TrackList TrackList=mb5_medium_get_tracklist(Medium);

											/* Another way of getting a string. Preallocate a buffer
											 * and check if if was big enough when retrieving string.
											 * If not, reallocate it to be big enough and get it again.
											 */

											RequiredSize=mb5_medium_get_title(Medium,MediumTitle,AllocSize);
											if (RequiredSize>AllocSize)
											{
												MediumTitle=realloc(MediumTitle,RequiredSize+1);
												mb5_medium_get_title(Medium,MediumTitle,RequiredSize+1);
											}

											printf("Found media: '%s', position %d\n",MediumTitle,mb5_medium_get_position(Medium));

											if (TrackList)
											{
												int ThisTrack=0;

												for (ThisTrack=0;ThisTrack<mb5_track_list_size(TrackList);ThisTrack++)
												{
													char *TrackTitle=0;
													int RequiredLength=0;

													Mb5Track Track=mb5_track_list_item(TrackList,ThisTrack);
													Mb5Recording Recording=mb5_track_get_recording(Track);

													/* Yet another way of getting string. Call it once to
													 * find out how long the buffer needs to be, allocate
													 * enough space and then call again.
													 */

													if (Recording)
													{
														RequiredLength=mb5_recording_get_title(Recording,TrackTitle,0);
														TrackTitle=malloc(RequiredLength+1);
														mb5_recording_get_title(Recording,TrackTitle,RequiredLength+1);
													}
													else
													{
														RequiredLength=mb5_track_get_title(Track,TrackTitle,0);
														TrackTitle=malloc(RequiredLength+1);
														mb5_track_get_title(Track,TrackTitle,RequiredLength+1);
													}

													printf("Track: %d - '%s'\n",mb5_track_get_position(Track),TrackTitle);

													free(TrackTitle);
												}
											}

											free(MediumTitle);
										}
									}
								}

								/* We must delete the result of 'media_matching_discid' */
								mb5_medium_list_delete(MediumList);
							}
						}

						/* We must delete anything returned from the query methods */
						mb5_metadata_delete(mb5md2);
					}

					free(ParamValues[0]);
					free(ParamValues);
					free(ParamNames[0]);
					free(ParamNames);
				}
			}
		}
		/* We must delete anything returned from the query methods */
		mb5_metadata_delete(mb5md1);
	}

	/* We must delete the original query object */
	mb5_query_delete(mb5q);
	discid_free(d);
	return identified_disc;
}


int main(int argc, char **argv)
{
	mcngd_calculate_cddb_id(TOC);

	return 0;
}
