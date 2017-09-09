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

#include <jansson.h>
#include <string.h>
#include "mcng-config.h"

#define MCNG_DEFAULT_MGT_SOCKET_LOCATION "/tmp/mcng-mgt.sock"
#define MCNG_DEFAULT_LOG_LEVEL "info"
#define MCNG_DEFAULT_LOG_FILE "/tmp/mcng.log"
#define MCNG_DEFAULT_USER "loewe"
#define MCNG_DEFAULT_GROUP "loewe"
#define MCNG_DEFAULT_PID_FILE "/tmp/mcng.pid"




static void mcng_add_defaut_config(mcng_config_t *conf)
{
	if(!conf)
	{
		return;
	}

	/* management */
	if(!conf->mgt_socket_location)
	{
		conf->mgt_socket_location = strdup(MCNG_DEFAULT_MGT_SOCKET_LOCATION);
	}
	/* logging */
	if(!conf->log_level)
	{
		conf->log_level = strdup(MCNG_DEFAULT_LOG_LEVEL);
	}
	if(!conf->log_file)
	{
		conf->log_file = strdup(MCNG_DEFAULT_LOG_FILE);
	}
	/* process */
	if(!conf->user)
	{
		conf->user = strdup(MCNG_DEFAULT_USER);
	}
	if(!conf->group)
	{
		conf->group = strdup(MCNG_DEFAULT_GROUP);
	}
	if(!conf->pid_file)
	{
		conf->pid_file = strdup(MCNG_DEFAULT_PID_FILE);
	}
}




static void mcng_json_read_communication(mcng_config_t *conf, json_t *j_communication)
{
	json_t *j_item = NULL;

	j_item = json_object_get(j_communication, "management_socket");
	if(j_item && json_is_string(j_item))
	{
		mcng_free_safe(conf->mgt_socket_location);
		conf->mgt_socket_location = strdup(json_string_value(j_item));
	}
}

static void mcng_json_read_logging(mcng_config_t *conf, json_t *j_logging)
{
	json_t *j_item = NULL;

	j_item = json_object_get(j_logging, "level");
	if(j_item && json_is_string(j_item))
	{
		mcng_free_safe(conf->log_level);
		conf->log_level = strdup(json_string_value(j_item));
	}

	j_item = json_object_get(j_logging, "file");
	if(j_item && json_is_string(j_item))
	{
		mcng_free_safe(conf->log_file);
		conf->log_file = strdup(json_string_value(j_item));
	}
}



static void mcng_json_read_process(mcng_config_t *conf, json_t *j_process)
{
	json_t *j_item = NULL;

	j_item = json_object_get(j_process, "user");
	if(j_item && json_is_string(j_item))
	{
		mcng_free_safe(conf->user);
		conf->user = strdup(json_string_value(j_item));
	}

	j_item = json_object_get(j_process, "group");
	if(j_item && json_is_string(j_item))
	{
		mcng_free_safe(conf->group);
		conf->group = strdup(json_string_value(j_item));
	}

	j_item = json_object_get(j_process, "pidfile");
	if(j_item && json_is_string(j_item))
	{
		mcng_free_safe(conf->pid_file);
		conf->pid_file = strdup(json_string_value(j_item));
	}
}



mcng_config_t *mcng_loadconfig(const char *filename)
{
	json_t *j = NULL, *j_cfg_section = NULL;
	json_error_t error;
	mcng_config_t *cfg = NULL;

	j = json_load_file(filename, 0, &error);
	if(!j)
	{
		printf("Cannot load the json file: `%s`, error: %s (%d)", filename, error.text, error.line);
		return NULL;
	}

	if(!json_is_object(j))
	{
    json_decref(j);
		return NULL;
	}
	if(NULL == (cfg = calloc(1, sizeof(mcng_config_t))))
	{
		json_decref(j);
		return NULL;
	}

	j_cfg_section = json_object_get(j, "process");
	if(j_cfg_section && json_is_object(j_cfg_section))
	{
		mcng_json_read_process(cfg, j_cfg_section);
	}

	j_cfg_section = json_object_get(j, "logging");
	if(j_cfg_section && json_is_object(j_cfg_section))
	{
		mcng_json_read_logging(cfg, j_cfg_section);
	}

	j_cfg_section = json_object_get(j, "communication");
	if(j_cfg_section && json_is_object(j_cfg_section))
	{
		mcng_json_read_communication(cfg, j_cfg_section);
	}
  json_decref(j);

	/* now we add default values for each unset item */
	mcng_add_defaut_config(cfg);

	return cfg;
}



void mcng_freeconfig(mcng_config_t *cfg)
{
	if(cfg)
	{
		/* process */
		mcng_free_safe(cfg->pid_file);
		mcng_free_safe(cfg->user);
		mcng_free_safe(cfg->group);
		/* communication */
		mcng_free_safe(cfg->mgt_socket_location);
		/* logging */
		mcng_free_safe(cfg->log_level);
		mcng_free_safe(cfg->log_file);
		/* the cfg structure itself */
		mcng_free_safe(cfg);
	}
}
