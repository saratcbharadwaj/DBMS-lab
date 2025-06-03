//Purpose: Implement a PDS system where a database is a directory containing an info file and multiple tables. Each table uses a BST for indexing records by key, supporting operations like creation, storage, retireval, and soft deletion.

// Key features: 
// 1. Directory-based database structure.
// 2. Multi-table support within a database.
// 3. BST-based inexing for O ( log n) key lookups.
// 4. Soft deletion via an is_deleted flag.
// 5. Persistence through .dat and .ndx files.

// Dependencies: 
// 1. bst.h/bst.c: Provides BST operations( bst_add_node, bst_search, bst_free, etc.)
// 2. contact.h: Defines struct Contact, struct Student and contact_name_matcher.
// 3. pdsv6.h: Defines struct PDS_DBInfo, struct PDS_TableInfo, struct PDS_NdxInfo, and constraints ( e.q PDS_SUCCESS, PDS_FILE_ERROR).

// includes and headers: 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "pdsv6.h"
#include "bst.h"
#include "contact.h"

//Global variable:
struct PDS_DBInfo repo_handle = {"", PDS_DB_CLOSED, 0};

// Purpose: manages the database state globally.
// Fields: 
//  1. repo_status: PDS_DB_OPEN or PDS_DB_CLOSED
//  2. num_tables: Number of tables.
//  3. tables: Array of struct PDS_TableInfo, each with:

//   a. table_name: Name
//   b. table_status: PDS_TABLE_OPEN or PDS_TABLE_CLOSED
//   c. rec_count: Number of records
//   d. ndx_root: BST root for indexing
//   e. data_fp: File pointer to .dat
//   f. rec_size: Size of each record.


//pds_create_db
int pds_create_db(char *dbname) {
    
    // Initialising a folder name
    char folder[50];
    
    snprintf(folder, sizeof(folder), "%s", dbname);
    // Now, creating an .info file in the above folder.
    struct stat st;
    if ( stat(folder, &st) == -1) {
        
        if ( mkdir(folder, 0777) == -1) {
            
            return PDS_FILE_ERROR;
        }
    }
    // Naming it up.
    char info_file[60];
    snprintf(info_file, sizeof(info_file), "%s/%s.info", dbname, dbname);
    // Accessing the .info file and then closing the file immediately.
    FILE *fp = fopen(info_file, "wb");
    if ( !fp) return PDS_FILE_ERROR;
    
    fwrite(dbname, sizeof(char), strlen(dbname) + 1, fp);
    fclose(fp);
    return PDS_SUCCESS;
}

int pds_open_db(char *dbname) {
    // Check if the database is already open.
    if ( repo_handle.repo_status == PDS_DB_OPEN) {
      
      return PDS_DB_ALREADY_OPEN;
    }
    // Change the directory to folder named "dbname".
    if (chdir(dbname) == -1)   {
    
        return PDS_FILE_ERROR;
    }
    // Open the file named dbname.info in "wb+" mode
    char info_file[60];
    
    snprintf(info_file, sizeof(info_file), "%s.info", dbname);
    
    FILE *fp = fopen(info_file, "wb+");
    if ( !fp) return PDS_FILE_ERROR;
    
    char name_check[30];
    fread(name_check, sizeof(char), strlen(dbname) + 1, fp);
    fclose(fp);
    
    if ( strcmp(name_check, dbname) != 0) {
      
        return PDS_FILE_ERROR;
    }
    // Update the status of PDS_DBInfo to PDS_REPO_OPEN
    repo_handle.repo_status = PDS_DB_OPEN;
    
    strcpy(repo_handle.pds_repo_name, dbname);
    return PDS_SUCCESS;
}

struct PDS_TableInfo *helper_get_table_info(char *table_name) {
    // Return the table_info from the array based on name.
    for (int i = 0; i < repo_handle.num_tables; i++) {
    
          if ( strcmp(repo_handle.tables[i].table_name, table_name) == 0) {
              
              return &repo_handle.tables[i];
        }
    }
    
    return NULL;
}

int pds_create_table(char *table_name) {

    if ( repo_handle.repo_status != PDS_DB_OPEN) {
        
        return PDS_DB_NOT_OPEN;
    }
    
    int i = repo_handle.num_tables;
    strcpy(repo_handle.tables[i].table_name, table_name);
    // Open the data file and index file in "wb" mode.
    char data_file[50], index_file[50];
    snprintf(data_file, sizeof(data_file), "%s.dat", table_name);
    snprintf(index_file, sizeof(index_file), "%s.ndx", table_name);
    
    FILE *data_fp = fopen(data_file, "wb");
    FILE *index_fp = fopen(index_file, "wb");
    
    if ( !data_fp || !index_fp) {

      if (data_fp) fclose(data_fp);
      if ( index_fp) fclose(index_fp);
      return PDS_FILE_ERROR;
    }
    // writing zero to the index file
    int zero = 0;
    if ( fwrite(&zero, sizeof(int), 1, index_fp) != 1) {
        
        fclose(data_fp);
        fclose(index_fp);
        return PDS_FILE_ERROR;
    }
    
    fclose(data_fp);
    fclose(index_fp);
    repo_handle.num_tables++;
    return PDS_SUCCESS;
}

int pds_open_table(char *table_name, int rec_size) {
    
    if ( repo_handle.repo_status != PDS_DB_OPEN) {
        
        return PDS_DB_NOT_OPEN;
    }
    struct PDS_TableInfo *t = helper_get_table_info(table_name);
    
    if ( !t || t->table_status == PDS_TABLE_OPEN) {
        
        return PDS_TABLE_NOT_OPEN;
    }
    
    char data_file[50], index_file[50];
    
    snprintf(data_file, sizeof(data_file), "%s.dat", table_name);
    snprintf(index_file, sizeof(index_file), "%s.ndx", table_name);
    
    t->data_fp = fopen(data_file, "rb+");
    t->ndx_fp = fopen(index_file, "rb+");
    
    if ( !t->data_fp || !t->ndx_fp) {
      
      
        if ( t->data_fp) fclose(t->data_fp);
        if ( t->ndx_fp) fclose(t->ndx_fp);
        
        return PDS_FILE_ERROR;
    }
    
    t->rec_size = rec_size;
    
    t->table_status = PDS_TABLE_OPEN;
    
    if ( pds_load_ndx(t) != PDS_SUCCESS) {
        
        fclose(t->data_fp);
        fclose(t->ndx_fp);
        return PDS_FILE_ERROR;
    }
    
    fclose(t->ndx_fp);
    t->ndx_fp = NULL;
    return PDS_SUCCESS;
}

int pds_load_ndx(struct PDS_TableInfo *tablePtr) {
      
    int num_records;
    
    if ( fread(&num_records, sizeof(int), 1, tablePtr->ndx_fp) != 1) {
          
          return PDS_FILE_ERROR;
    }
    tablePtr->rec_count = num_records;
    for (int i =0; i < num_records; i++) { 
        
        struct PDS_NdxInfo *ndx = malloc(sizeof(struct PDS_NdxInfo));
        if ( fread(ndx, sizeofstruct PDS-NdxInfo), 1, tablePtr->ndx_fp) != 1) {
            
            free(ndx);
            return PDS_FILE_ERROR;
        }
        
        if ( bst_ad_node(&tablePtr->ndx_root, ndx->key, ndx) != BST_SUCCESS) {
            
            free(ndx);
            return PDS_FILE_ERROR;
        }
  }
  
  return PDS_FILE_ERROR;
}

int put_rec_by_key(char *table_name, int key, void* rec) { 
    
    
    struct PDS_TableInfo *t = helper_get_table_info(table_name);
    
    if (!t || t->table_status != PDS_TABLE_OPEN) {
        
        return PDS_TABLE_NOT_OPEN;
    }
    
    






