#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pdsv3.h"

struct PDS_RepoInfo repo_handle;

int pds_create(char *repo_name) {
    char file_name[50];
    snprintf(file_name, sizeof(file_name), "%s.dat", repo_name);

    char index_file[50];
    snprintf(index_file, sizeof(index_file), "%s.ndx", repo_name);

    FILE *data_fp = fopen(file_name, "wb");
    FILE *index_fp = fopen(index_file, "wb");

    if (!data_fp || !index_fp) {
        if (data_fp) fclose(data_fp);
        if (index_fp) fclose(index_fp);
        return PDS_FILE_ERROR;
    }

    int z = 0;
    fwrite(&z, sizeof(int), 1, index_fp);
    fclose(data_fp);
    fclose(index_fp);
    return PDS_SUCCESS;
}

int put_rec_by_key(int key, void *rec) {
    if (repo_handle.repo_status != PDS_REPO_OPEN) {
        return PDS_FILE_ERROR;
    }

    // Check if the key already exists
    for (int i = 0; i < repo_handle.rec_count; i++) {
        if (repo_handle.ndx_array[i].key == key) {
            return PDS_ADD_FAILED;
        }
    }

    // Move the file pointer to the end of the data file to append the record
    fseek(repo_handle.pds_data_fp, 0, SEEK_END);
    int offset = ftell(repo_handle.pds_data_fp);

    // Write the key and the record data to the file
    if (fwrite(&key, sizeof(int), 1, repo_handle.pds_data_fp) != 1 ||
        fwrite(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp) != 1) {
        return PDS_FILE_ERROR;
    }

    // Reallocate the index array to accommodate the new record
    struct PDS_NdxInfo *new_ndx_array = (struct PDS_NdxInfo *)realloc(repo_handle.ndx_array, (repo_handle.rec_count + 1) * sizeof(struct PDS_NdxInfo));
    if (!new_ndx_array) {
        return PDS_FILE_ERROR;
    }

    // Update the index array with the new record info
    repo_handle.ndx_array = new_ndx_array;
    repo_handle.ndx_array[repo_handle.rec_count].key = key;
    repo_handle.ndx_array[repo_handle.rec_count].offset = offset;
    repo_handle.rec_count++;

    return PDS_SUCCESS;
}



int put_rec_by_key(int key, void *rec) {
    if (repo_handle.repo_status != PDS_REPO_OPEN) {
        return PDS_FILE_ERROR;
    }

    for (int i = 0; i < repo_handle.rec_count; i++) {
        if (repo_handle.ndx_array[i].key == key) {
            return PDS_ADD_FAILED;
        }
    }

    fseek(repo_handle.pds_data_fp, 0, SEEK_END);
    int offset = ftell(repo_handle.pds_data_fp);

    if (fwrite(&key, sizeof(int), 1, repo_handle.pds_data_fp) != 1 ||
        fwrite(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp) != 1) {
        return PDS_FILE_ERROR;
    }

    repo_handle.ndx_array = realloc(repo_handle.ndx_array, (repo_handle.rec_count + 1) * sizeof(struct PDS_NdxInfo));
    if (!repo_handle.ndx_array) {
        return PDS_FILE_ERROR;
    }

    repo_handle.ndx_array[repo_handle.rec_count].key = key;
    repo_handle.ndx_array[repo_handle.rec_count].offset = offset;
    repo_handle.rec_count++;

    return PDS_SUCCESS;
}

int get_rec_by_key(int key, void *rec) {
    if (repo_handle.repo_status != PDS_REPO_OPEN) {
        return PDS_REPO_NOT_OPEN;
    }

    for (int i = 0; i < repo_handle.rec_count; i++) {
        if (repo_handle.ndx_array[i].key == key) {
            fseek(repo_handle.pds_data_fp, repo_handle.ndx_array[i].offset + sizeof(int), SEEK_SET);
            if (fread(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp) != 1) {
                return PDS_REC_NOT_FOUND;
            }
            return PDS_SUCCESS;
        }
    }
    return PDS_REC_NOT_FOUND;
}

