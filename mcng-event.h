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

#ifndef __MLGUARD_MCNG_EVENT_H
#define __MLGUARD_MCNG_EVENT_H

/* flag to set epoll instance edge-triggered */
#define MCNG_EV_EDGE 1
/* flag to set epoll instance in one-shot mode */
#define MCNG_EV_SINGLE 2


typedef struct mcng_event_context_t mcng_event_context_t;
typedef struct mcng_event_t mcng_event_t;


struct mcng_event_t
{
	/* the handle/socket to the polling instance */
  int fd;
	/* the flag mask that has been used during creation */
	unsigned int flags;
	/* the event context pointer */
	mcng_event_context_t *ev_ctx;
	/* user pointer for use in the callback functions */
	void *user_ptr;
	/* this function is called once the POLLIN event occurred */
	int (*read_function)(mcng_event_t *ev);
	/* this function is called once the POLLOUT event occurred */
	int (*write_function)(mcng_event_t *ev);
};


/* mcng_event_init
 *
 * create an instance of epoll for event handling
 */
mcng_event_context_t *mcng_event_init(void);


/* mcng_event_add_event
 *
 * add an event watch to the event queue.
 * ev: context created by mcng_event_init()
 * handle: socket or other compatible handle to watch for
 * flags: flags MCNG_EV_* if required
 * read_function: function that will be called once POLLIN event triggered (or NULL if no POLLIN is to be watched)
 * write_function: function that will be called once POLLOUT event triggered (or NULL if no POLLOUT is to be watched)
 * data: pointer to user data to be used in callback functions
 */
mcng_event_t *mcng_event_add_event(mcng_event_context_t *ev, int handle, unsigned int flags, int (*read_function)(mcng_event_t *ev), int (*write_function)(mcng_event_t *ev), void *data);


/* mcng_event_cleanup
 *
 * close event instance and free used memory
 */
void mcng_event_cleanup(mcng_event_context_t *ev_ctx);

#endif
