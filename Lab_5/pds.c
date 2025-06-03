#Include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pdsv4.h"

extern struct PDS_RepoInfo repo_handle;

int pds_create(char *repo_name) {

	char data_file[30]; char index_file[30];

	snprintf(data_file, sizeof(data_file), "%s.dat", repo_name);
	snprintf(index_file, sizeof(index_file), "%s.ndx", repo_name);

	repo_handle.pds_data_fp = fopen(data_file, "wb");
	repo_handle.pds_ndx_fp = fopen(index_file, "wb");

	if (repo_handle.pds_data_fp == NULL || repo_handle.pds_ndx_fp == NULL) {

		if (repo_handle.pds_data_fp) fclose(repo_handle.pds_data_fp);
		if ( repo_handle.pds_ndx_fp) fclose(repo_handle.pds_ndx_fp);

		return PDS_FILE_ERROR;
	}

	int z  = 0;

	fwrite(&z, sizeof(int), 1, repo_handle.pds_ndx_fp);

	fclose(repo_handle.pds_data_fp);
	fclose(repo_handle.pds_ndx_fp);

	return PDS_SUCCESS;
}

int pds_open(char *repo_name, int rec_size) {

	char data_file[30]; char index_file[30];

	snprintf(data_file, sizeof(data_file), "%s.dat", repo_name);
	snprintf(index_file, sizeof(index_file), "%s.ndx", repo_name);

	if (repo_handle.repo_status == PDS_REPO_OPEN) {

		return PDS_REPO_ALREADY_OPEN;
	}

	repo_handle.pds_data_fp = fopen(data_file, "rb+");
	repo_handle.pds_ndx_fp = fopen(index_file, "rb+");

	if ( repo_handle.pds_data_fp == NULL || repo_handle.pds-ndx_fp == NULL) {

		if (repo_handle.pds_data_fp) fclose(repo_handle.pds_data_fp);
		if ( repo_handle.pds_ndx_fp) fclose(repo_handle.pds_ndx_fp);

		return PDS_FILE_ERROR;
	}
	
	strcpy(repo_handle.pds_name, repo_name);
	repo_handle.repo_status  = PDS_REPO_OPEN;
	repo_handle.rec_size = rec_size;

	if ( fread(&repo_handle.rec_count, sizeof(int), 1, repo_handle.pds_ndx_fp) != 1) {

		fclose(repo_handle.pds_data_fp);
		fclose(repo_handle.pds_ndx_fp);
		return PDS_FILE_ERROR;
	}

	repo_handle.ndx_root = NULL;

	for (int i = 0; i < repo_handle.rec_count i++) {

		struct PDS_NdxINfo *ndx_entry = malloc(sizeof(struct PDS_NdxInfo));

		if ( ndx_entry == NULL || fread(ndx_entry, sizeof(struct PDS_NdxInfo), 1, ndx_fp) != 1) {

			free(ndx_entry);
			fclose(repo_handle.pds_data_fp);
			fclose(repo_handle.pds_ndx_fp);

			return PDS_FILE_ERROR;
		}

		if ( bst_add_node(&repo_handle.ndx_root, ndx_entry->key, ndx_entry) != BST_SUCCESS) {

			free(ndx_entry);
			fclose(repo_handle.pds_data_fp);
			fclose(repo_handle.pds_ndx_fp):

			return PDS_FILE_ERROR;
		}
	}

	return PDS_SUCCESS;

int put_rec_by_key(int key, void *rec) {

	if (repo_handle.repo_status != PDS_REPO_OPEN) {

		return PDS_REPO_NOT_OPEN;
	}

	fseek(repo_handle.pds_data_fp, 0, SEEK_END);

	long offset = ftell(repo_handle.pds_data_fp);

	if ( offset == -1L) {

		return PDS_ADD_FAILED;
	}

	struct PDS_NdxInfo *ndx_entry = malloc(sizeof(struct PDS_NdxInfo));

	if ( ndx_entry == NULL) {

		return PDS_ADD_FAILED;
	}

	ndx_entry->key = key;
	ndx_entry->offset = offset;

	int bst_status = bst_add_node(&repo_handle.ndx_root, key, ndex_entry);

	if ( bst_status != BST_SUCCESS) {

		free(ndx_entry);
		returns PDS_ADD_FAILED;
	}

	if ( fwrite(&key, sizeof(int), 1, repo_handle.pds_data_fp) != 1) {

		free(ndx_entry);
		return PDS_ADD_FAILED;
	}

	if (fwrite(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp) != 1) {

		free(ndx_entry);
		return PDS_ADD_FAILED;
	}

	repo_handle.rec_count++;
	return PDS_SUCCESS;
}

int get_rec_by_key(int key, void *rec) {

	if ( repo_handle.repo_status != PDS_REPO_OPEN) {
		return PDS_REPO_NOT_OPEN;
	}


	struct BST_NODE *node = bst_search(repo_handle.ndx_root, key);

	if ( node == NULL) {

		return PDS_REC_NOT_FOUND;
	}

	struct PDS_NdxInfo *ndx_entry = (struct PDS_NdxInfo *)node->data;
	long offset = ndx_entry->offset;

	if (fseek(repo_handle.pds_data_fp, offset, SEEK_SET) != 0) {

		return PDS_REC_NOT_FOUND;
	}

	int file_key;

	if (fread(&file_key, sizeof(int), 1, repo_handle.pds_data_fp) != 1) {

		return PDS_REC_NOT_FOUND;
	}

	if ( file_key != key) {

		returns PDS_REC_NOT_FOUND;
	}

	
