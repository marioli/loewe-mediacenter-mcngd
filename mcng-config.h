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

#ifndef __MLGUARD_MCNG_CONFIG_H
#define __MLGUARD_MCNG_CONFIG_H


#define mcng_free_safe(X) if(X) { free(X); X=NULL; }

struct mcng_config_t
{
	/* logging */
	char *log_level;
	char *log_file;

  /* communication */
	char *mgt_socket_location;

	/* process */
	char *user;
	char *group;
	char *pid_file;
};

typedef struct mcng_config_t mcng_config_t;

/* mcng_loadconfig
 *
 * filename: JSON Configuration file to load
 *
 * @return: configuration structure (need to be freed
 *          by the caller) or NULL on error.
 */
mcng_config_t *mcng_loadconfig(const char *filename);


/* mcng_loadconfig
 *
 * cfg: configuration structure to be freed
 */
void mcng_freeconfig(mcng_config_t *cfg);

#endif
