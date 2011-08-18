/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2005 by Miika Pekkarinen
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/
#ifndef _DIRCACHE_H
#define _DIRCACHE_H

#include "config.h"
#include "dir_uncached.h"
#include <string.h> /* size_t */

#ifdef HAVE_DIRCACHE

#define DIRCACHE_RESERVE  (1024*64)
#define DIRCACHE_LIMIT    (1024*1024*6)

#define DIRCACHE_APPFLAG_TAGCACHE  0x0001
#define DIRCACHE_APPFLAG_PLAYLIST  0x0002

/* Internal structures. */
struct travel_data {
    struct dircache_entry *first;
    struct dircache_entry *ce;
    struct dircache_entry *down_entry;
#if (CONFIG_PLATFORM & PLATFORM_HOSTED)
    DIR_UNCACHED *dir, *newdir;
    struct dirent_uncached *entry;
#else
    struct fat_dir *dir;
    struct fat_dir newdir;
    struct fat_direntry entry;
#endif
    int pathpos;
};

struct dirent_cached {
    struct dirinfo info;
    char *d_name;
    long startcluster;
};

typedef struct {
    bool busy;
    struct dirent_cached theent; /* .attribute is set to -1 on init(opendir) */
    int internal_entry;          /* the current entry in the directory */
    DIR_UNCACHED *regulardir;
} DIR_CACHED;

void dircache_init(void) INIT_ATTR;
int dircache_load(void);
int dircache_save(void);
int dircache_build(int last_size);
void* dircache_steal_buffer(size_t *size);
bool dircache_is_enabled(void);
bool dircache_is_initializing(void);
void dircache_set_appflag(long mask);
bool dircache_get_appflag(long mask);
int dircache_get_entry_count(void);
int dircache_get_cache_size(void);
int dircache_get_reserve_used(void);
int dircache_get_build_ticks(void);
void dircache_disable(void);
int dircache_get_entry_id(const char *filename);
size_t dircache_copy_path(int index, char *buf, size_t size);

/* the next two are internal for file.c */
long _dircache_get_entry_startcluster(int id);
struct dirinfo* _dircache_get_entry_dirinfo(int id);

void dircache_bind(int fd, const char *path);
void dircache_update_filesize(int fd, long newsize, long startcluster);
void dircache_update_filetime(int fd);
void dircache_mkdir(const char *path);
void dircache_rmdir(const char *path);
void dircache_remove(const char *name);
void dircache_rename(const char *oldpath, const char *newpath);
void dircache_add_file(const char *path, long startcluster);

DIR_CACHED* opendir_cached(const char* name);
struct dirent_cached* readdir_cached(DIR_CACHED* dir);
int closedir_cached(DIR_CACHED *dir);
int mkdir_cached(const char *name);
int rmdir_cached(const char* name);
#endif /* !HAVE_DIRCACHE */

#endif
