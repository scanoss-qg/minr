#ifndef __MINR_H
    #define _MINR_H
// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * src/minr.h
 *
 * Global minr declarations
 *
 * Copyright (C) 2018-2021 SCANOSS.COM
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Definitions */
#define MINR_VERSION "2.1.0"
#define FILE_FILES 256
#define MAX_ARG_LEN 1024
#define MIN_FILE_REC_LEN 70
#define MAX_PATH_LEN 4096
#define MAX_FILE_SIZE (4 * 1048576)
#define MAX_FILE_HEADER 4096
#define MAX_CSV_LINE_LEN 1024
#define BUFFER_SIZE 1048576
#define MIN_FILE_SIZE 256 // files below this size will be ignored
#define DISCARD_PATH_IF_LONGER_THAN 1024
#define MD5_LEN 16
#define MD5_LEN_HEX 32
#define NODE_PTR_LEN 5
#define REC_SIZE_LEN 2
#define LDB_KEY_LN 4
#define MAX_COPYRIGHT_LEN 256
#define MAX_NONALNUM_IN_COPYRIGHT 10 // Maximum amount of non alphanumeric bytes accepted in a copyright statement
#define MIN_ALNUM_IN_COPYRIGHT 3 // Minimum amount of alphanumeric bytes required in a copyright statement

/* Set best practices to twice the average (calculated over entire OSSKB) */
#define BEST_PRACTICES_MAX_LINES (210 * 2)
#define BEST_PRACTICES_MAX_LINE_LN (36 * 2)
#define BEST_PRACTICES_MAX_LINES_PER_COMMENT (27 * 2)

#define MAX_LICENSE_ID 64
#define MAX_LICENSE_TEXT 1024

/* Structures */
typedef struct normalized_license
{
  char spdx_id[MAX_LICENSE_ID];
  char text[MAX_LICENSE_TEXT];
  int ln;
} normalized_license;

struct minr_job
{
	char path[MAX_PATH_LEN];
	char url[MAX_ARG_LEN];
	char urlid[MD5_LEN * 2 + 1];
	char fileid[MD5_LEN * 2 + 1];
	char metadata[MAX_ARG_LEN];
	char mined_path[MAX_ARG_LEN]; // Location of output mined/ directory
	char tmp_dir[MAX_ARG_LEN];    // Temporary directory for decompressing files
	bool all_extensions;
	bool exclude_mz;
	bool exclude_detection;

	// minr -i
	char import_path[MAX_PATH_LEN];
	bool skip_sort; // Do not sort before importing

	// minr -f -t
	char join_from[MAX_PATH_LEN];
	char join_to[MAX_PATH_LEN];

	// minr -z
	char mz[MAX_PATH_LEN];

	// Memory allocation
	char *src; // for uncompressed source
	uint8_t *zsrc; // for compressed source
	uint64_t src_ln;
	uint64_t zsrc_ln;
	struct mz_cache_item *mz_cache; // for mz cache

	normalized_license *licenses; // Array of known license identifiers
	int license_count;            // Number of known license identifiers
};

typedef enum { none, license, copyright, quality } metadata;

/* Pointers */
uint8_t *buffer;
uint32_t *hashes, *lines;

/* File descriptor arrays */
FILE *out_component;
FILE **out_file;
int *out_snippet;

extern char tmp_path[MAX_ARG_LEN];
extern int min_file_size;

bool check_dependencies(void);
bool download(struct minr_job *job);
void recurse(struct minr_job *job, char *path);
void minr_join(struct minr_job *job);
void minr_join_mz(char *source, char *destination);
void mine_license(char *mined_path, char *md5, char *src, uint64_t src_ln, normalized_license *licenses, int license_count);
void mine_copyright(char *mined_path, char *md5, char *src, uint64_t src_ln);
void mine_quality(char *mined_path, char *md5, char *src, long size);
normalized_license *load_licenses(int *count);

#endif
