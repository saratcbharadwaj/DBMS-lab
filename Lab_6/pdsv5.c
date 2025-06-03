// copy pds_close() from lab5
// make changes in get_rec_by_field()
// change in get_rec_by_key()
// copy load_ndx() 
// copy pds_open(), 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pdsv5.h"

struct PDS_RepoInfo repo_handle;

int pds_create(char *repo_name) {
    
    char file_name[30], index_file[30];
    
    strcpy(file_name, repo_name); strcat(file_name, ".dat");
    strcpy(index_file, repo_name); strcat(index_file, ".ndx");

    repo_handle.pds_data_fp = fopen(file_name, "wb");
    repo_handle.pds_ndx_fp = fopen(index_file, "wb");
   
   if (repo_handle.pds_data_fp == NULL || repo_handle.pds_ndx_fp == NULL) {
   
   return PDS_FILE_ERROR;
    }

    int z = 0;
   
   fwrite(&z, sizeof(int), 1, repo_handle.pds_ndx_fp);
    fclose(repo_handle.pds_data_fp);
    fclose(repo_handle.pds_ndx_fp);

    return PDS_SUCCESS;
}

int pds_open(char *repo_name, int rec_size) {
   
    char file_name[30];
    char index_file[30];
    
    strcpy(file_name, repo_name); strcat(file_name, ".dat");
    strcpy(index_file, repo_name); strcat(index_file, ".ndx");
    
    if (repo_handle.repo_status == PDS_REPO_OPEN) {
    
        return PDS_REPO_ALREADY_OPEN;
    }

    
    repo_handle.pds_data_fp = fopen(file_name, "rb+");
    repo_handle.pds_ndx_fp = fopen(index_file, "rb+");

    if (repo_handle.pds_data_fp == NULL || repo_handle.pds_ndx_fp == NULL) {
        
        if (repo_handle.pds_data_fp) {
        
            fclose(repo_handle.pds_data_fp); 
        }
        
        if (repo_handle.pds_ndx_fp) {
            
            fclose(repo_handle.pds_ndx_fp); 
        } 
        
        return PDS_FILE_ERROR;
    }
    strcpy(repo_handle.pds_name, repo_name);
    repo_handle.repo_status = PDS_REPO_OPEN;
    repo_handle.rec_size = rec_size;


    int status = pds_load_ndx();
    if (status != PDS_SUCCESS) {
    
        fclose(repo_handle.pds_data_fp); 
        fclose(repo_handle.pds_ndx_fp); 
        
        return PDS_FILE_ERROR;         
    }

    fclose(repo_handle.pds_ndx_fp);
    repo_handle.pds_ndx_fp = NULL;

    return PDS_SUCCESS;
}


int put_rec_by_key(int key, void *rec) {

    if (repo_handle.repo_status != PDS_REPO_OPEN) {
    
      return PDS_REPO_NOT_OPEN;
    }

    if (bst_search(repo_handle.ndx_root, key) != NULL) {
        
        return PDS_ADD_FAILED; 
    }

    if (fseek(repo_handle.pds_data_fp, 0, SEEK_END) != 0) {
        
        return PDS_ADD_FAILED;
    }
    
    long offset = ftell(repo_handle.pds_data_fp);
    
    if (offset == -1L) {
    
        return PDS_ADD_FAILED;
    }
    // index entry creation 
    struct PDS_NdxInfo *ndx_entry = malloc(sizeof(struct PDS_NdxInfo));
    
    if (ndx_entry == NULL) {
    
        return PDS_ADD_FAILED; 
    }
    ndx_entry->is_deleted = REC_NOT_DELETED;
    
    ndx_entry->key = key;       
    ndx_entry->offset = offset; 

    // Adding Index 
    int bst_status = bst_add_node(&repo_handle.ndx_root, key, ndx_entry);
    
    if (bst_status != BST_SUCCESS) {
    
        free(ndx_entry);
        return PDS_ADD_FAILED; 
    }

    // writing into data file
    if (fwrite(&key, sizeof(int), 1, repo_handle.pds_data_fp) != 1) {
    
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

    if (repo_handle.repo_status != PDS_REPO_OPEN) {
        return PDS_REPO_NOT_OPEN;
    }

    // searching for index key
    struct BST_Node *node = bst_search(repo_handle.ndx_root, key);
    if (node == NULL) {
    
        return PDS_REC_NOT_FOUND; 
    }

    struct PDS_NdxInfo *ndx_entry = (struct PDS_NdxInfo *)node->data;
    long offset = ndx_entry->offset;
    if (ndx_entry->is_deleted == REC_DELETED) {

        return PDS_REC_NOT_FOUND;
        }
        

    if (fseek(repo_handle.pds_data_fp, offset, SEEK_SET) != 0) {
    
        return PDS_REC_NOT_FOUND; 
    }

    int file_key;
    if (fread(&file_key, sizeof(int), 1, repo_handle.pds_data_fp) != 1) {
    
        return PDS_REC_NOT_FOUND; 
    }

    if (file_key != key) {
    
        return PDS_REC_NOT_FOUND; 
    }

    if (fread(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp) != 1) {
    
        return PDS_REC_NOT_FOUND; 
    }int pds_load_ndx() {
  
    if (repo_handle.pds_ndx_fp == NULL) {
        return PDS_FILE_ERROR;
    }


    int rec_count;
    if (fread(&rec_count, sizeof(int), 1, repo_handle.pds_ndx_fp) != 1) {
        return PDS_FILE_ERROR;
    }

    repo_handle.rec_count = rec_count;
    repo_handle.ndx_root = NULL;

    // reading indexes.
    for (int i = 0; i < rec_count; i++) {
        struct PDS_NdxInfo *ndx_entry = malloc(sizeof(struct PDS_NdxInfo));
        if (fread(ndx_entry, sizeof(struct PDS_NdxInfo), 1, repo_handle.pds_ndx_fp) != 1) {
            free(ndx_entry);
            return PDS_FILE_ERROR;
        }

        int status = bst_add_node(&repo_handle.ndx_root, ndx_entry->key, ndx_entry);
        if (status != BST_SUCCESS) {
            free(ndx_entry);
            return PDS_FILE_ERROR;
        }
    }

    return PDS_SUCCESS;
}


    return PDS_SUCCESS;
}

int get_rec_by_field(void *searchvalue, void *rec, int (*matcher)(void *rec, void *searchvalue), int *comp_count) {
   
    if (repo_handle.repo_status != PDS_REPO_OPEN) {
        return PDS_REPO_NOT_OPEN;
    }

    rewind(repo_handle.pds_data_fp);  /
    *comp_count = 0; 

    int key;
    
    
    while (fread(&key, sizeof(int), 1, repo_handle.pds_data_fp) == 1) {

       
        if (fread(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp) != 1) {
            return PDS_FILE_ERROR;  
        }

       
        (*comp_count)++;

       
        if (matcher(rec, searchvalue) == 0) {
            
            struct BST_Node *node = bst_search(repo_handle.ndx_root, key);
            if (node != NULL) {
                struct PDS_NdxInfo *ndx_entry = (struct PDS_NdxInfo *)node->data;
                
                // If the record is deleted, continue searching
                if (ndx_entry->is_deleted == REC_DELETED) {
                    continue; 
                }
            }

            return PDS_SUCCESS;
        }
    }

    return PDS_REC_NOT_FOUND;
}


int delete_rec_by_key(int key) {
    if (repo_handle.repo_status != PDS_REPO_OPEN) {
        return PDS_REPO_NOT_OPEN;  // Repository not open
    }

    struct BST_Node *node = bst_search(repo_handle.ndx_root, key);
    if (node == NULL) {
        return PDS_REC_NOT_FOUND;  // Key not found
    }

    struct PDS_NdxInfo *ndx_entry = (struct PDS_NdxInfo *)node->data;
    if (ndx_entry->is_deleted == REC_NOT_DELETED) {
        ndx_entry->is_deleted = REC_DELETED;  // Mark as deleted
        return PDS_SUCCESS;
    } else {
        return PDS_REC_NOT_FOUND;  // Already marked deleted
    }
}

int pds_close(char *repo_name) {
    
    if (strcmp(repo_name, repo_handle.pds_name) != 0) {
          return PDS_FILE_ERROR;
    }

    char index_file[30];
    strcpy(index_file, repo_name);
    strcat(index_file, ".ndx");

    if (repo_handle.repo_status == PDS_REPO_CLOSED) {
        
        return PDS_REPO_CLOSED;
    }

    repo_handle.pds_ndx_fp = fopen(index_file, "wb");

    if (repo_handle.pds_ndx_fp == NULL) {
        
        return PDS_FILE_ERROR;
    }

    fwrite(&repo_handle.rec_count, sizeof(int), 1, repo_handle.pds_ndx_fp);

    void write_to_file(struct BST_Node *node) {
        
        if (node == NULL) return;
        fwrite(node->data, sizeof(struct PDS_NdxInfo), 1, repo_handle.pds_ndx_fp);
        write_to_file(node->left_child);
        write_to_file(node->right_child);
    }

    write_to_file(repo_handle.ndx_root);

    if (fclose(repo_handle.pds_data_fp) != 0) {
        return PDS_FILE_ERROR;
    }

    if (fclose(repo_handle.pds_ndx_fp) != 0) {
        return PDS_FILE_ERROR;
    }

    bst_destroy(repo_handle.ndx_root);
    repo_handle.ndx_root = NULL;

    repo_handle.repo_status = PDS_REPO_CLOSED;

    return PDS_SUCCESS;
}


int pds_load_ndx() {
  
    if (repo_handle.pds_ndx_fp == NULL) {
        return PDS_FILE_ERROR;
    }


    int rec_count;
    if (fread(&rec_count, sizeof(int), 1, repo_handle.pds_ndx_fp) != 1) {
        return PDS_FILE_ERROR;
    }

    repo_handle.rec_count = rec_count;
    repo_handle.ndx_root = NULL;

    // reading indexes.
    for (int i = 0; i < rec_count; i++) {
        struct PDS_NdxInfo *ndx_entry = malloc(sizeof(struct PDS_NdxInfo));
        if (fread(ndx_entry, sizeof(struct PDS_NdxInfo), 1, repo_handle.pds_ndx_fp) != 1) {
            free(ndx_entry);
            return PDS_FILE_ERROR;
        }

        int status = bst_add_node(&repo_handle.ndx_root, ndx_entry->key, ndx_entry);
        if (status != BST_SUCCESS) {
            free(ndx_entry);
            return PDS_FILE_ERROR;
        }
    }

    return PDS_SUCCESS;
}

