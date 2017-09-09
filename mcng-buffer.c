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
*/

#include <stdlib.h>
#include "mcng-buffer.h"


struct mcng_buffer_t
{
  /* current length of buffer (used bytes) */
  size_t len;     
  /* maximum length of buffer (presently allocated) */
  size_t limit;   
  /* actual data pointer */
  char *data;     
};




mcng_buffer_t *mcng_buffer_size(mcng_buffer_t *buf, size_t len)
{
  if(buf == NULL)
  {
    buf = malloc(sizeof(struct mcng_buffer_t));
    buf->data = NULL;
    buf->len = 0;
  }
  buf->data = realloc(buf->data, len);
  if(buf->len > len)
  {
    buf->len = len;
  }
  buf->limit = len;
  return buf;
}


void mcng_buffer_concat(mcng_buffer_t *dst, const char *src, size_t len)
{
  size_t i;

  for(i = 0; i < len; i++)
  {
    dst->data[dst->len++] = src[i];
  }
}


char *mcng_buffer_to_str(mcng_buffer_t *buf)
{
  char *str = malloc(buf->len + 1);
  memcpy(str, buf->data, buf->len);
  str[buf->len] = 0;
  return str;
}



void mcng_buffer_free(mcng_buffer_t *buf)
{
	if(buf)
	{
		if(buf->data)
		{
			free(buf->data);
		}
		free(buf);
	}
}
