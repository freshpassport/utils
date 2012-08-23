#include <errno.h>
#include <stdio.h>
#include <limits.h>	// for PATH_MAX
#include <stdint.h>
#include <stdbool.h>

#ifndef _LIB_UDV_H
#define _lib_UDV_H

/**
 * Defination
 */

#define MAX_UDV 10

typedef enum _udv_state udv_state;
enum _udv_state {
        UDV_RAW = 0,
        UDV_ISCSI,
        UDV_NAS
};

typedef struct _udv_info udv_info_t;

#define UDV_NAME_LEN 72
struct _udv_info {
        char name[UDV_NAME_LEN];
        char vg_dev[PATH_MAX];
        int part_num;
        uint64_t start, end, capacity;
        uint32_t sector_size;   // not used currently
        udv_state state;
};


/**
 * API
 */

ssize_t udv_create(const char *vg_name, const char *name, uint64_t capacity);

ssize_t udv_delete(const char *name);

size_t udv_list(udv_info_t *list, size_t n);

ssize_t udv_rename(const char *name, const char *new_name);

/**
 * Utils
 */
bool udv_exist(const char *name);

const char* vg_name2dev(const char *name);

const char* vg_dev2name(const char *dev);

const char* udv_name2dev(const char *name);

const char* dev_dev2name(const char *dev);

#endif/*_LIB_UDV_H*/
