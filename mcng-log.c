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

#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>
#include <string.h>


#include "mcng-log.h"



static const char *log_names[5] = {"debug", "info", "warning", "error", "fatal"};

#define ISO8601_FMT(ltime, tv) "%04d-%02d-%02dT%02d:%02d:%02d.%03d%s",   \
    (ltime)->tm_year+1900, (ltime)->tm_mon+1, (ltime)->tm_mday,               \
    (ltime)->tm_hour, (ltime)->tm_min, (ltime)->tm_sec, (int)(tv)->tv_usec/ 1000, \
    tzname[0]

struct mcng_log_t
{
	FILE *log_file_handle;
	char *filename;
	int pid;
	enum mcng_log_level_t log_level;
};

static int mcng_log_timestamp(char *buf, int len)
{
	struct timeval tv;
	struct tm *ltime;
	int used_len = 0;

	gettimeofday(&tv, NULL);
	ltime = localtime(&tv.tv_sec);
	used_len += snprintf(buf, len, ISO8601_FMT(ltime, &tv));
	used_len += snprintf(buf + used_len, len - used_len, ": ");
	return used_len;
}


mcng_log_t *mcng_log_open(const char *logfile_name, unsigned int pid, const char *log_level)
{
	mcng_log_t *log_ctx = NULL;
	enum mcng_log_level_t test_level = 0;

	if(NULL == (log_ctx = malloc(sizeof(mcng_log_t))))
	{
		return NULL;
	}
	if(logfile_name)
	{
		/* real name means we are dealing with a real file */
		log_ctx->filename = strdup(logfile_name);
		if(NULL == (log_ctx->log_file_handle = fopen(logfile_name, "r+")))
		{
			if(NULL == (log_ctx->log_file_handle = fopen(logfile_name, "w+"))) 
			{
				fprintf(stderr, "cannot open file %s for writing\n", logfile_name);
				free(log_ctx);
				return NULL;
			}
		}
	}
	else
	{
		/* NULL as logfile_name is defined for us to log to stderr */
		log_ctx->filename = NULL;
		log_ctx->log_file_handle = stderr;
	}
	log_ctx->pid = pid;

	/* set log_level */
	if(!log_level)
	{
		/* NULL means "debug" for easy use */
		log_ctx->log_level = MC_LOG_DEBUG;
	}
	else
	{
		test_level = MC_LOG_LEVEL_MAX;
		while(test_level)
		{
			test_level--;
			if(0 == strcmp(log_names[test_level], log_level))
			{
				break;
			}
		}
		log_ctx->log_level = test_level;
	}

	return log_ctx;
}


void mcng_log_write(mcng_log_t *log_ctx, enum mcng_log_level_t level, const char *fmt, ...)
{
	char buf[1024];
	int buf_used = 0;
	va_list ap;
	va_start(ap, fmt);

	if(!log_ctx || level > 4)
	{
		return;
	}
	if(log_ctx->log_level >= level)
	{
		return;
	}

	buf_used += mcng_log_timestamp(buf, sizeof(buf));
	buf_used += snprintf(buf + buf_used, sizeof(buf) - buf_used, "%s: [%d] ", log_names[level], log_ctx->pid);
	buf_used += vsnprintf(buf + buf_used, sizeof(buf) - buf_used, fmt, ap);

	va_end(ap);

	fprintf(log_ctx->log_file_handle, "%s\n", buf);
}


void mcng_log_reopen(mcng_log_t *log_ctx)
{
	if(log_ctx && log_ctx->filename)
	{
		/* close and open log file for log-rotation */
		fclose(log_ctx->log_file_handle);
		if(NULL == (log_ctx->log_file_handle = fopen(log_ctx->filename, "a+")))
		{
			/* unable to re-open log. unusual but can happen. lets hope stderr is still seen by someone */
			log_ctx->log_file_handle = stderr;
			free(log_ctx->filename);
			log_ctx->filename = NULL;
		}
	}

}


void mcng_log_close(mcng_log_t *log_ctx)
{
	if(log_ctx)
	{
		if(log_ctx->filename)
		{
			fclose(log_ctx->log_file_handle);
			free(log_ctx->filename);
		}
		free(log_ctx);
	}
}

