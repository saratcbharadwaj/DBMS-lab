#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "pdsv6.h"
#include "bst.h"
//BT2024221

#include "bst.h"
#include "contact.h"
#include "pdsv6.h"

struct PDS_DBInfo repo_handle;

//////////////////////////////////////////////////////////////////////////////
// pds_create_db
int pds_create_db(char *dbname) {
    char folder[50];
    snprintf(folder, sizeof(folder), "%s", dbname);

    // Check if folder exists, create if it doesn't
    struct stat st;
    if (stat(folder, &st) == -1) {
        if (mkdir(folder, 0777) == -1) {
            printf("mkdir failed: %s\n", strerror(errno));
            return PDS_FILE_ERROR;
        }
    } else if (!S_ISDIR(st.st_mode)) {
        printf("'%s' exists but is not a directory\n", folder);
        return PDS_FILE_ERROR;
    }

    char file_name[60];
    snprintf(file_name, sizeof(file_name), "%s/%s.info", dbname, dbname);

    FILE *infofile = fopen(file_name, "wb");
    if (infofile == NULL) {
        printf("fopen failed: %s\n", strerror(errno));
        return PDS_FILE_ERROR;
    }

    fwrite(dbname, sizeof(char), strlen(dbname) + 1, infofile);
    fclose(infofile);

    return PDS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
// pds_open_db
int pds_open_db(char *dbname){
    // change to db folder
    int k = chdir(dbname);
    if(k == -1){
        printf("here 1");
        // folder change failed
        return PDS_FILE_ERROR;
    }
    char info_file[30];
    FILE *info;
    // check if db already open
    if(repo_handle.repo_status==PDS_DB_OPEN){
        return PDS_DB_ALREADY_OPEN;
    }
    // copy db name
    strcpy(info_file, dbname);
    // add .info extension
    strcat(info_file, ".info");
    // open info file
    info = fopen(info_file, "rb+");
  
    // set db status to open
    repo_handle.repo_status = PDS_DB_OPEN;
    char db_name2[30];
    // read db name from file
    (fread(db_name2, sizeof(char), strlen(dbname)+1, info)!=1);
    // close info file
    fclose(info);  
    // check if names match
    if(strcmp(db_name2, dbname) != 0){ 
        printf("here 2");
        return PDS_FILE_ERROR;
    }

    // all good
    return PDS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
// helper_get_table_info
struct PDS_TableInfo *helper_get_table_info(char *table_name) {
    // loop through tables
    for (int i = 0; i < repo_handle.num_tables; i++) {
        // check if table name matches
        if (strcmp(repo_handle.tables[i].table_name, table_name) == 0) {
            // return table info pointer
            return &repo_handle.tables[i];
        }
    }
    // table not found
    return NULL;
}

//////////////////////////////////////////////////////////////////////////////
// pds_create_table
int pds_create_table(char *table_name) {
    // get next table index
    int index = repo_handle.num_tables;

    // copy table name
    strcpy(repo_handle.tables[index].table_name, table_name);

    // set initial table properties
    repo_handle.tables[index].table_status = PDS_TABLE_CLOSED;
    repo_handle.tables[index].rec_count = 0;
    repo_handle.tables[index].ndx_root = NULL;

    char data_file[30], ndx_file[30];
    // create data file name
    snprintf(data_file, sizeof(data_file), "%s.dat", table_name);
    // create index file name
    snprintf(ndx_file, sizeof(ndx_file), "%s.ndx", table_name);

    // open files for writing
    repo_handle.tables[index].data_fp = fopen(data_file, "wb");
    repo_handle.tables[index].ndx_fp = fopen(ndx_file, "wb");
    // check if files opened
    if (repo_handle.tables[index].data_fp == NULL || repo_handle.tables[index].ndx_fp == NULL) {
        if (repo_handle.tables[index].data_fp != NULL)
            // close data file if opened
            fclose(repo_handle.tables[index].data_fp);
        if (repo_handle.tables[index].ndx_fp != NULL)
            // close index file if opened
            fclose(repo_handle.tables[index].ndx_fp);
        return PDS_FILE_ERROR;
    }

    // write zero to index file
    int zero = 0;
    if (fwrite(&zero, sizeof(int), 1, repo_handle.tables[index].ndx_fp) != 1) {
        // clean up on write fail
        fclose(repo_handle.tables[index].data_fp);
        fclose(repo_handle.tables[index].ndx_fp);
        return PDS_FILE_ERROR;
    }

    // close both files
    fclose(repo_handle.tables[index].data_fp);
    fclose(repo_handle.tables[index].ndx_fp);

    // increment table count
    repo_handle.num_tables++;

    // all good
    return PDS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
// pds_load_ndx
int pds_load_ndx(struct PDS_TableInfo *tablePtr) {
    int num_records, i;
    struct PDS_NdxInfo ndx_entry;

    // read number of records
    if (fread(&num_records, sizeof(int), 1, tablePtr->ndx_fp) != 1) {
        return PDS_FILE_ERROR;
    }

    // loop to load index entries
    for (i = 0; i < num_records; i++) {
        // read one index entry
        if (fread(&ndx_entry, sizeof(struct PDS_NdxInfo), 1, tablePtr->ndx_fp) != 1) {
            return PDS_FILE_ERROR;
        }
        // add to bst
        if (bst_add_node(&(tablePtr->ndx_root), ndx_entry.key, &ndx_entry) != 0) {
            return PDS_FILE_ERROR;
        }
    }
    // all good
    return PDS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
// pds_open_table
int pds_open_table(char *table_name, int rec_size) {
    // get table info
    struct PDS_TableInfo *table = helper_get_table_info(table_name);
    if (table == NULL) {
        return PDS_TABLE_NOT_OPEN;
    }

    // check if already open
    if (table->table_status == PDS_TABLE_OPEN) {
        return PDS_DB_ALREADY_OPEN;
    }

    char data_file[30], ndx_file[30];
    // create file names
    snprintf(data_file, sizeof(data_file), "%s.dat", table_name);
    snprintf(ndx_file, sizeof(ndx_file), "%s.ndx", table_name);

    // open files for read/write
    table->data_fp = fopen(data_file, "rb+");
    table->ndx_fp = fopen(ndx_file, "rb+");
    if (table->data_fp == NULL || table->ndx_fp == NULL) {
        if (table->data_fp != NULL) fclose(table->data_fp);
        if (table->ndx_fp != NULL) fclose(table->ndx_fp);
        return PDS_FILE_ERROR;
    }

    // set record size and status
    table->rec_size = rec_size;
    table->table_status = PDS_TABLE_OPEN;

    // load index
    if (pds_load_ndx(table) != PDS_SUCCESS) {
        fclose(table->data_fp);
        fclose(table->ndx_fp);
        return PDS_FILE_ERROR;
    }

    // close index file
    fclose(table->ndx_fp);
    table->ndx_fp = NULL;

    // all good
    return PDS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
// put_rec_by_key
int put_rec_by_key(char *table_name, int key, void *rec) {
    // get table info
    struct PDS_TableInfo *table = helper_get_table_info(table_name);
    if (table == NULL || table->table_status != PDS_TABLE_OPEN) {
        return PDS_TABLE_NOT_OPEN;
    }
    if (table->data_fp == NULL) {
        return PDS_DB_NOT_OPEN;
    }

    struct PDS_NdxInfo ndx_entry;
    long offset;

    // move to end of file
    if (fseek(table->data_fp, 0, SEEK_END) != 0) {
        return PDS_ADD_FAILED;
    }

    // get current position
    offset = ftell(table->data_fp);
    if (offset == -1) {
        return PDS_ADD_FAILED;
    }

    // set index entry values
    ndx_entry.key = key;
    ndx_entry.offset = offset;
    ndx_entry.is_deleted = REC_NOT_DELETED;

    // add to bst
    if (bst_add_node(&(table->ndx_root), key, &ndx_entry) != 0) {
        return PDS_ADD_FAILED;
    }

    // increment record count
    table->rec_count++;

    // write key
    if (fwrite(&key, sizeof(int), 1, table->data_fp) != 1) {
        return PDS_ADD_FAILED;
    }

    // write record
    if (fwrite(rec, table->rec_size, 1, table->data_fp) != 1) {
        return PDS_ADD_FAILED;
    }

    // all good
    return PDS_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
// get_rec_by_key
int get_rec_by_key(char *table_name, int key, void *rec) {
    // get table info
    struct PDS_TableInfo *table = helper_get_table_info(table_name);
    if (table == NULL || table->table_status != PDS_TABLE_OPEN) {
        return PDS_TABLE_NOT_OPEN;
    }
    // search bst for key
    struct BST_Node *node = bst_search(table->ndx_root, key);
    if (node == NULL) {
        return PDS_REC_NOT_FOUND;
    }
    // get index info
    struct PDS_NdxInfo *ndx_info = (struct PDS_NdxInfo *)node->data;
    if (ndx_info->is_deleted == REC_DELETED) {
        return PDS_REC_NOT_FOUND;
    }
    // seek to record position
    if (fseek(table->data_fp, ndx_info->offset, SEEK_SET) != 0) {
        return PDS_FILE_ERROR;
    }
    int read_key;
    // read key
    if (fread(&read_key, sizeof(int), 1, table->data_fp) != 1) {
        return PDS_FILE_ERROR;
    }
    // read record
    if (fread(rec, table->rec_size, 1, table->data_fp) != 1) {
        return PDS_FILE_ERROR;
    }
    // all good
    return PDS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
// get_rec_by_field
int get_rec_by_field(char *table_name, void *searchvalue, void *rec,int (*matcher)(void *rec, void *searchvalue), int *comp_count) {
    // get table info
    struct PDS_TableInfo *table = helper_get_table_info(table_name);
    if (table == NULL || table->table_status != PDS_TABLE_OPEN) {
        return PDS_TABLE_NOT_OPEN;
    }

    // start from beginning
    fseek(table->data_fp, 0, SEEK_SET);
    int read_key, search_count = 0;
    *comp_count = 0;

    // loop through all records
    while (fread(&read_key, sizeof(int), 1, table->data_fp) == 1) {
        search_count++;
        // read record
        if (fread(rec, table->rec_size, 1, table->data_fp) != 1) {
            return PDS_FILE_ERROR;
        }
        (*comp_count)++;
        // check if record matches
        if (matcher(rec, searchvalue) == 0) {
            // check if not deleted
            struct BST_Node *node = bst_search(table->ndx_root, read_key);
            if (node != NULL) {
                struct PDS_NdxInfo *ndx_info = (struct PDS_NdxInfo *)node->data;
                if (ndx_info->is_deleted == REC_NOT_DELETED) {
                    return PDS_SUCCESS;
                }
            }
        }
    }

    // record not found
    return PDS_REC_NOT_FOUND;
}

//////////////////////////////////////////////////////////////////////////////
// delete_rec_by_key
int delete_rec_by_key(char *table_name, int key) {
    // get table info
    struct PDS_TableInfo *table = helper_get_table_info(table_name);
    if (table == NULL || table->table_status != PDS_TABLE_OPEN) {
        return PDS_TABLE_NOT_OPEN;
    }
    // search for key
    struct BST_Node *node = bst_search(table->ndx_root, key);
    if (node == NULL) {
        return PDS_REC_NOT_FOUND;
    }
    // get index info
    struct PDS_NdxInfo *ndx_info = (struct PDS_NdxInfo *)node->data;
    // check deletion status
    if (ndx_info->is_deleted == REC_NOT_DELETED) {
        // mark as deleted
        ndx_info->is_deleted = REC_DELETED;
        return PDS_SUCCESS;
    } else {
        return PDS_REC_NOT_FOUND;
    }
}

//////////////////////////////////////////////////////////////////////////////
// pds_close
int pds_close_table(char *table_name) {
    // get table info
    struct PDS_TableInfo *table = helper_get_table_info(table_name);
    if (table == NULL || table->table_status != PDS_TABLE_OPEN) {
        return PDS_TABLE_NOT_OPEN;
    }

    char ndx_file[30];
    // create index file name
    snprintf(ndx_file, sizeof(ndx_file), "%s.ndx", table_name);
    // open index file
    table->ndx_fp = fopen(ndx_file, "wb");
    if (table->ndx_fp == NULL) {
        return PDS_NDX_SAVE_FAILED;
    }

    // write record count
    if (fwrite(&table->rec_count, sizeof(int), 1,table->ndx_fp) != 1) {
        fclose(table->ndx_fp);
        return PDS_NDX_SAVE_FAILED;
    }

    // print bst (debug)
    bst_print(table->ndx_root);

    // free bst
    bst_free(table->ndx_root);
    table->ndx_root = NULL;
    // close files
    fclose(table->ndx_fp);
    fclose(table->data_fp);

    // set table closed
    table->table_status = PDS_TABLE_CLOSED;

    // all good
    return PDS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
// pds_close_db
int pds_close_db(char *dbname) {
    // loop through all tables
    for (int i = 0; i < repo_handle.num_tables; i++) {
        // close open tables
        if (repo_handle.tables[i].table_status == PDS_TABLE_OPEN) {
            pds_close_table(repo_handle.tables[i].table_name);
        }
    }
    // go to parent directory
    chdir("..");
    // set db closed
    repo_handle.repo_status = PDS_DB_CLOSED;

    // all good
    return PDS_SUCCESS;
}
