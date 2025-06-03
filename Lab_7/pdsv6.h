#ifndef PDS_H
#define PDS_H

#include "bst.h"

// Error codes
#define PDS_SUCCESS 0
#define PDS_FILE_ERROR 1
#define PDS_ADD_FAILED 2
#define PDS_LOAD_NDX_FAILED 3
#define PDS_REC_NOT_FOUND -1
#define PDS_DB_ALREADY_OPEN 12
#define PDS_NDX_SAVE_FAILED 13
#define PDS_DB_NOT_OPEN 14
#define PDS_TABLE_NOT_OPEN 15
#define PDS_DELETE_FAILED 16

// Repository status values
#define PDS_DB_OPEN 101
#define PDS_DB_CLOSED 102
#define PDS_TABLE_OPEN 103
#define PDS_TABLE_CLOSED 104


// Constant to be used for array implementation of index
#define MAX_NDX_SIZE 1000
#define MAX_TABLES 5

// Deletion status values
#define REC_DELETED 1
#define REC_NOT_DELETED 0

// Structure of the index entry
struct PDS_NdxInfo{
	int key;
	int offset;
	int is_deleted;  // Can be REC_DELETED or REC_NOT_DELETED
};

// [NEW]
struct PDS_TableInfo{
	char table_name[50];
	FILE *data_fp;
	FILE *ndx_fp;
	int table_status; // PDS_TABLE_OPEN or PDS_TABLE_CLOSED
	int rec_size;
	int rec_count;
	struct BST_Node *ndx_root;
};

// [NEW]
struct PDS_DBInfo{
	char pds_repo_name[30];
	int repo_status; 
	int num_tables;
	struct PDS_TableInfo tables[MAX_TABLES];
};

extern struct PDS_DBInfo repo_handle;

// [NEW]
//////////////////////////////////////////////////////////////////////////////
// pds_create_db
//
// Create a new folder named repo_name
// Function to use: mkdir
//
// Create file named repo_name/repo_name.info
//
// Store the name of database into the info file
// Function to use: fwrite
// close the files
// NOTHING need on PDS_DBInfo
//
// Return values:
//
// PDS_FILE_ERROR - if mkdir returns -1
// PDS_SUCCESS - function completes without any error
//
int pds_create_db( char *dbname );

// [NEW]
//////////////////////////////////////////////////////////////////////////////
// pds_open_db
//
// Change the director to folder named dbname
// Function to use: chdir
//
// Open the file named dbname.info in wb+
// Update the staus of PDS_DBInfo to PDS_REPO_OPEN
//
// Read the name of database from the info file
// Function to use: fread
//
// Compare value read from info file matches the input dbname
//
// Return values:
//
// PDS_DB_ALREADY_OPEN - if open is being attempted on a db that is already open
// PDS_FILE_ERROR - if chdir returns -1
// PDS_FILE_ERROR - if dbname in info does not match with given name
// PDS_SUCCESS - function completes without any error
//
int pds_open_db( char *dbname );

//////////////////////////////////////////////////////////////////////////////
// helper_get_table_info
//
// Function return the table_info from the array based on name
//
// Return values:
// Pointer to tableinfo - Use & to return the appropriate element from the array
// NULL - if table_name is not found in the tables array of DBInfo
//
struct PDS_TableInfo *helper_get_table_info( char *table_name );

//////////////////////////////////////////////////////////////////////////////
// pds_create_table
//
// Open the data file and index file in "wb" mode
// Function to use: fopen
//
// Write 0 to index file
// Function to use: fwrite/ 
//
// No updates needed to table needed to PDS_TableInfo
//
// Return values:
//
// PDS_DB_NOT_OPEN - if database is not open
// PDS_FILE_ERROR - if fopen returns NULL
// PDS_FILE_ERROR - if fwrite fails
//
// PDS_SUCCESS - function completes without any error
//
// [OLD PROTOTYPE] int pds_create(char *repo_name);
int pds_create_table( char *table_name );

//////////////////////////////////////////////////////////////////////////////
// pds_open_table
// Open the data file and index file in rb+ mode
// Function to use: fopen
// Update the fields of PDS_TableInfo appropriately
// Call pds_load_ndx to load the index
// Close only the index file
// Function to use: fclose
//
// Return values:
//
// PDS_DB_NOT_OPEN - if database is not open
// PDS_FILE_ERROR - if fopen returns NULL
// PDS_TABLE_ALREADY_OPEN - if table_status is PDS_TABLE_OPEN
// PDS_SUCCESS - function completes without any error
// int pds_open( char *repo_name, int rec_size );
int pds_open_table( char *table_name, int rec_size );

//////////////////////////////////////////////////////////////////////////////
// pds_load_ndx
// This is Internal function used by pds_open
// Read the number of records form the index file
// Set up a loop to load the index entries into the BST ndx_root by calling bst_add_node repeatedly for each 
// index entry. Unlike array where you could load entire index, for BST, you add index entries
// one by one by reading the index file one by one in a loop
// (see bst_demo.c how to call bst_add_node in a loop)
// Function to use: fread in a loop
// Function to use: bst_add_node in the same loop
//
// Return values:
//
// PDS_FILE_ERROR - if the number of entries is lesser than the number of records
// PDS_SUCCESS - function completes without any error
//
// [OLD-PROTOTYPE] int pds_load_ndx();
int pds_load_ndx( struct PDS_TableInfo *tablePtr );

//////////////////////////////////////////////////////////////////////////////
// put_rec_by_key
// Seek to the end of the data file
// Function to use: fseek
// Identify the current location of file pointer
// Function to use: ftell
// Create new index entry using the key and ftell value
// [CHANGE] set the is_deleted flag to REC_NOT_DELETED
// Add index entry to BST by calling bst_add_node (see bst_demo.c how to call bst_add_node)
// Function to use: bst_add_node
// Increment record count
// Write the key
// Function to use: fwrite
// Write the record
// Function to use: fwrite
//
// Return values:
//
// PDS_DB_NOT_OPEN - if database is not open
// PDS_TABLE_NOT_OPEN - if table status is not PDS_TABLE_OPEN
// PDS_ADD_FAILED - if fwrite fails OR duplicate record is found
// PDS_SUCCESS - function completes without any error
// [OLD PROTOTYPE] int put_rec_by_key( int key, void *rec );
int put_rec_by_key( char *table_name, int key, void *rec );

//////////////////////////////////////////////////////////////////////////////
// get_rec_by_key
// Search for index entry in BST
// Function to use: bst_search
// 
// Type cast the void *data in the node to struct PDS_NdxInfo *
// [CHANGE] Return PDS_REC_NOT_FOUND if is_deleted_flag is REC_DELETED
// Seek to the file location based on offset in index entry
// Function to use: fseek
// Read the key at the current location 
// Function to use: fread
// Read the record from the current location
// Function to use: fread
//
// Return values:
//
// PDS_DB_NOT_OPEN - if database is not open
// PDS_TABLE_NOT_OPEN - if table status is not PDS_TABLE_OPEN
// PDS_REC_NOT_FOUND - if the key is not found or is_deleted_flag is REC_DELETED
// PDS_SUCCESS - function completes without any error
// [OLD PROTOTYPE] int get_rec_by_key( int key, void *rec );
int get_rec_by_key( char *table_name, int key, void *rec );

//////////////////////////////////////////////////////////////////////////////
// get_rec_by_field
// Brute-force retrieval using an arbitrary search value
// 	search_count = 0
// 	fread key from data file until EOF
//	search_count++
//  fread the record
//  Invoke the matcher using current record and search value
//	if mathcher returns success, return the current record only if not deleted, else continue the loop
// end loop
//
// Return values:
//
// PDS_DB_NOT_OPEN - if database is not open
// PDS_TABLE_NOT_OPEN - if table status is not PDS_TABLE_OPEN
// PDS_REC_NOT_FOUND - if the record is not found or is_deleted_flag is REC_DELETED
// PDS_SUCCESS - function completes without any error
// comp_count should be the number of comparisons being done in the linear search
// 
// [OLD PROTOTYPE] int get_rec_by_field(void *searchvalue, void *rec, int (*matcher)(void *rec, void *searchvalue), int *comp_count);
int get_rec_by_field( char *table_name, void *searchvalue, void *rec, int (*matcher)(void *rec, void *searchvalue), int *comp_count);

//////////////////////////////////////////////////////////////////////////////
// delete_rec_by_key
//
// Search for index entry in BST
// Function to use: bst_search
// Type cast the void *data in the node to struct PDS_NdxInfo *
// if is_deleted_flag is REC_NOT_DELETED
// 		Set the is_deleted flag to REC_DELETED
// else
// 		return PDS_REC_NOT_FOUND
//
// Return values:
//
// PDS_DB_NOT_OPEN - if database is not open
// PDS_TABLE_NOT_OPEN - if table status is not PDS_TABLE_OPEN
// PDS_REC_NOT_FOUND - if the key is not found or is already marked as REC_DELETED
// PDS_SUCCESS - function completes without any error
// 
// [OLD PROTOTYPE] int delete_rec_by_key( int key );
int delete_rec_by_key( char *table_name, int key );

//////////////////////////////////////////////////////////////////////////////
// pds_close
//
// Open the index file in wb mode (write mode, not append mode)
// Function to use: fopen
// Store the number of records
// Function to use: fwrite
// Unload the index into the index file by traversing the BST in pre-order mode (overwrite the entire index file)
// See function bst_print in bst.c to see how to do tree traversal
// Function to use: fwrite
// Free the BST and set pointer to NULL
// Function to use: bst_free
// Close the index file and data file
// Function to use: fclose
//
// Return values:
//
// PDS_DB_NOT_OPEN - If database is not open
// PDS_TABLE_NOT_OPEN - if table status is not PDS_TABLE_OPEN
// PDS_NDX_SAVE_FAILED - if fopen or fwrite fails
// PDS_SUCCESS - function completes without any error
// [OLD PROTOTYPE] int pds_close( char * repo_name );
int pds_close_table( char *table_name );

// [NEW]
//////////////////////////////////////////////////////////////////////////////
// pds_close_db
//
// Loop through PDS_TableInfo array
// If the table status is OPEN, then call pds_close_table
// Change the folder to parent folder by calling chdir("..")
int pds_close_db( char *dbname );

#endif
