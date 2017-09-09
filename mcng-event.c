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

#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "mcng-event.h"


struct mcng_event_context_t
{
	int fd;
};



mcng_event_context_t *mcng_event_init()
{
	mcng_event_context_t *ev_ctx = NULL;
	int current_flags;

	ev_ctx = malloc(sizeof(mcng_event_context_t));
	if(NULL == ev_ctx)
	{
		return NULL;
	}

	ev_ctx->fd = epoll_create(1);
	if(ev_ctx->fd < 0)
	{
		free(ev_ctx);
		return NULL;
	}
	/* i would prefer epoll_create1() but it was introduced in
	 * kernel 2.6.27 and the mediacenter has a lower version :(
	 */
  current_flags = fcntl(ev_ctx->fd, F_GETFD, 0);
	if(current_flags >= 0)
	{
		if(fcntl(ev_ctx->fd, F_SETFD, current_flags | FD_CLOEXEC) < 0)
		{
			/* just visible in debug mode */
			fprintf(stderr, "unable to set socket FD_CLOEXEC flag (err = %d)\n", errno);
		}
	}
	return ev_ctx;
}


mcng_event_t *mcng_event_add_event(mcng_event_context_t *ev_ctx, int handle, unsigned int flags, int (*read_function)(mcng_event_t *ev), int (*write_function)(mcng_event_t *ev), void *data)
{
	mcng_event_t *new_event = NULL;
	struct epoll_event ev;

	new_event = malloc(sizeof(mcng_event_t));
	if(NULL == new_event)
	{
		return NULL;
	}
	new_event->fd = handle;
	new_event->flags = flags;
	new_event->ev_ctx = ev_ctx;
	new_event->user_ptr = data;
	new_event->read_function = read_function;
	new_event->write_function = write_function;

	/* fill epoll_event struct */
	ev.events = 0;
	if(read_function)
	{
	  ev.events |= EPOLLIN;
	}
	if(write_function)
	{
	  ev.events |= EPOLLOUT;
	}
	if(flags & MCNG_EV_EDGE)
	{
	  ev.events |= EPOLLET;
	}
	if(flags & MCNG_EV_SINGLE)
	{
	  ev.events |= EPOLLONESHOT;
	}
	ev.data.ptr = new_event;

	/* add event to kernel */
	if(0 != epoll_ctl(ev_ctx->fd, EPOLL_CTL_ADD, handle, &ev))
	{
		/* just visible in debug mode */
		fprintf(stderr, "unable to add epoll event: %d\n", errno);
		free(new_event);
		new_event = NULL;
	}

	return new_event;
}


int mcng_event_loop(mcng_event_context_t *ev_ctx)
{
	int num_fds, curr_event;
	struct epoll_event new_events[10];
	mcng_event_t *ev_ptr;


  /* wait up to 1 second */
	num_fds = epoll_wait(ev_ctx->fd, new_events, 10, 1000);

	if(num_fds < 0)
	{
		fprintf(stderr, "epoll_wait() returned an error: %d\n", errno);
	}
	else
	{
		curr_event = 0;
		while(curr_event < num_fds)
		{
			ev_ptr = new_events[curr_event].data.ptr;
			if(new_events[curr_event].events & EPOLLIN && ev_ptr->read_function != NULL)
			{
				/* call read_function as read event has occurred */
				ev_ptr->read_function(ev_ptr);
			}
			if(new_events[curr_event].events & EPOLLOUT && ev_ptr->write_function != NULL)
			{
				/* call write_function as write event has occurred */
				ev_ptr->write_function(ev_ptr);
			}
			num_fds++;
		}
	}
	return 0;
}


void mcng_event_cleanup(mcng_event_context_t *ev_ctx)
{
	if(ev_ctx)
	{
	  close(ev_ctx->fd);
		free(ev_ctx);
	}
}
