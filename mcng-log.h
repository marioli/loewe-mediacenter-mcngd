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
#ifndef __MLGUARD_MCNG_LOG_H
#define __MLGUARD_MCNG_LOG_H

struct mcng_log_t;
typedef struct mcng_log_t mcng_log_t;

enum mcng_log_level_t
{
	MC_LOG_DEBUG = 0,
	MC_LOG_INFO,
	MC_LOG_WARN,
	MC_LOG_ERROR,
	MC_LOG_FATAL,
	MC_LOG_LEVEL_MAX
};

/* mcng_log_open
 * opens logfile_name (appending) and creates a log context
 * that is configured to log everything in the named log_level (or higher)
 * into the file. The pid is written into each line for convenience.
 * 
 * logfile_name: file name (absolute or relative) to the log file or NULL for stderr
 *
 * pid: process ID to be logged in each line of logging
 *
 * log_level: log level name ("debug", "info", "warning", ...) or NULL for default (= debug)
 *
 * @return: mcng_log_t pointer (needs to be freed by caller)
 */
mcng_log_t *mcng_log_open(const char *logfile_name, unsigned int pid, const char *log_level);



/* mcng_log_write
 * this is the function that actual will write the output messages into the given log context
 * (file) as long as the level is equal or lower to the requested log level.
 * 
 * log_ctx: pointer to log context
 * 
 * level: enum mcng_log_level_t (0-4) for log level
 *
 * fmt: log content (like printf())
 */
void mcng_log_write(mcng_log_t *log_ctx, enum mcng_log_level_t level, const char *fmt, ...);


/* mcng_log_reopen
 * closes log file and opens it again. This is required on linux to
 * give logrotate the chance to have a log rotation. As the process
 * keeps the file handle (even after renaming).
 *
 * log_ctx: pointer to log context
 */
void mcng_log_reopen(mcng_log_t *log_ctx);



/* mcng_log_close
 * closes log file and tidy up resources (free()).
 * 
 * log_ctx: pointer to log context
 */
void mcng_log_close(mcng_log_t *log_ctx);


/* macros for convenience */
#define mcng_log_debug(ctx, format, ...) mcng_log_write(ctx, MC_LOG_DEBUG, format, ## __VA_ARGS__)
#define mcng_log_info(ctx, format, ...) mcng_log_write(ctx, MC_LOG_INFO, format, ## __VA_ARGS__)
#define mcng_log_warn(ctx, format, ...) mcng_log_write(ctx, MC_LOG_WARN, format, ## __VA_ARGS__)
#define mcng_log_error(ctx, format, ...) mcng_log_write(ctx, MC_LOG_ERROR, format, ## __VA_ARGS__)
#define mcng_log_fatal(ctx, format, ...) mcng_log_write(ctx, MC_LOG_FATAL, format, ## __VA_ARGS__)


#endif
