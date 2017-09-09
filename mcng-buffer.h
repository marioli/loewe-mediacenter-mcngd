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
#ifndef __MLGUARD_MCNG_BUFFER_H
#define __MLGUARD_MCNG_BUFFER_H

#include <string.h>
#include <stdint.h>

typedef struct mcng_buffer_t mcng_buffer_t;

/* mcng_buffer_size
 *
 * create or resize a buffer
 */
mcng_buffer_t *mcng_buffer_size(mcng_buffer_t *buf, size_t len);


/* mcng_buffer_concat
 *
 * add len amount of chars in src to the buffer dst
 * the len is not checked and is required to fit into the buffer(!!)
 */
void mcng_buffer_concat(mcng_buffer_t *dst, const char *src, size_t len);


/* mcng_buffer_to_str
 *
 * export the buffer as null terminated string. The string is allocated
 * in the heap and needs to be freed by the caller.
 */
char *mcng_buffer_to_str(mcng_buffer_t *buf);


/* mcng_buffer_free
 *
 * free the buffer structure
 */
void mcng_buffer_free(mcng_buffer_t *buf);


#endif
