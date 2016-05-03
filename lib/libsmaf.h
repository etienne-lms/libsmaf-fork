/*
 * libsmaf.h
 *
 * Copyright (C) Linaro SA 2015
 * Author: Benjamin Gaignard <benjamin.gaignard@linaro.org> for Linaro.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335  USA
 */

#ifndef _LIBSMAF_H_
#define _LIBSMAF_H_

/* For O_CLOEXEC and O_RDWR flags definition */
#include <fcntl.h>

/* For close() */
#include <unistd.h>

#if defined(__cplusplus)
extern "C" {
#endif

int smaf_open();
void smaf_close();

int smaf_create_buffer(unsigned int length, unsigned int flags, char *name, int *fd);
int smaf_set_secure(int fd, int secure);
int smaf_get_secure(int fd);

/* get number of registered allocators */
int smaf_allocator_count();

/* return alloctor name per index
 * it is up to caller to release returned value
 */
char *smaf_get_allocator_name(int index);

#if defined(__cplusplus)
}
#endif

#endif
