#ifndef PDS_H
#define PDS_H

#include "bst.h"

// Error codes
#define PDS_SUCCESS 0
#define PDS_FILE_ERROR 1
#define PDS_ADD_FAILED 2
#define PDS_REC_NOT_FOUND -1
#define PDS_REPO_ALREADY_OPEN 12
#define PDS_NDX_SAVE_FAILED 13
#define PDS_REPO_NOT_OPEN 14

// Repository status values
#define PDS_REPO_OPEN 10
#define PDS_REPO_CLOSED 11

#define MAX_NDX_SIZE 1000

struct PDS_NdxInfo{
	int key;
	int offset;
};

struct PDS_RepoInfo{
	char pds_name[30];
	FILE *pds_data_fp;
	FILE *pds_ndx_fp;
	int repo_status; 
	int rec_size; // For fixed length records
	int rec_count; // For the number of records in ndx_array
	// Array index is replaced with Binary Search Tree (BST)
	// struct PDS_NdxInfo ndx_array[MAX_NDX_SIZE];
	struct BST_Node *ndx_root;
};

extern struct PDS_RepoInfo repo_handle;

//////////////////////////////////////////////////////////////////////////////
// pds_create
// Open the data file and index file in "wb" mode
// Function to use: fopen
// Initialize index file by storing "0" to indicate there are zero entries in index file
// Function to use: fwrite
// close the files
//
// Return values:
//
// PDS_FILE_ERROR - if fopen returns NULL
// PDS_SUCCESS - function completes without any error
//
int pds_create(char *repo_name);

//////////////////////////////////////////////////////////////////////////////
// pds_open
// Open the data file and index file in rb+ mode
// Function to use: fopen
// Update the fields of PDS_RepoInfo appropriately
// Call pds_load_ndx to load the index
// Close only the index file
// Function to use: fclose
//
// Return values:
//
// PDS_FILE_ERROR - if fopen returns NULL
// PDS_REPO_ALREADY_OPEN - if repo_status is PDS_REPO_OPEN
// PDS_SUCCESS - function completes without any error

// Purpose: Opens an existing repository
// Steps: 
//  1. Generates filenames for the data file and index file
// 2. Opens both files in read-write mode
// 3. Checks if the repo is already open
// 4. Loads the index entries from the index file into the BST using pds_load_ndx.
// 5. CLoses the index file after loading.

int pds_open( char *repo_name, int rec_size );

//////////////////////////////////////////////////////////////////////////////
// pds_load_ndx
// This is Internal function used by pds_open
// Read the number of records form the index file
// [CHANGE] Set up a loop to load the index entries into the BST ndx_root by calling bst_add_node repeatedly for each 
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
int pds_load_ndx();

//////////////////////////////////////////////////////////////////////////////
// put_rec_by_key
// Seek to the end of the data file
// Function to use: fseek
// Identify the current location of file pointer
// Function to use: ftell
// Create new index entry using the key and ftell value
// [CHANGE] Add index entry to BST by calling bst_add_node (see bst_demo.c how to call bst_add_node)
// Function to use: bst_add_node
// Increment record count
// Write the key
// Function to use: fwrite
// Write the record
// Function to use: fwrite
//
// Return values:
//
// PDS_REPO_NOT_OPEN - if repo_status is not PDS_REPO_OPEN
// PDS_ADD_FAILED - if fwrite fails OR duplicate record is found
// PDS_SUCCESS - function completes without any error

// Purpose: Adds a new record to the rpeo
// Steps: 
// 1. Checks if the repo is open
// 2. Searches the BST to ensure the key is unique
// 3. Moves the file pointer to the end of the data file.
// 4. Creates an index entry with the key and offset.
// 5. Adds the index entry to the BST.
// 6. Writes the key and record to the data file.
// 7. Incrememtns the record count.
// Returns: 
//  PDS_SUCCESS
int put_rec_by_key( int key, void *rec );

//////////////////////////////////////////////////////////////////////////////
// get_rec_by_key
// [CHANGE] Search for index entry in BST
// Function to use: bst_search
// Seek to the file location based on offset in index entry
// Function to use: fseek
// Read the key at the current location 
// Function to use: fread
// Read the record from the current location
// Function to use: fread
//
// Return values:
//
// PDS_REPO_NOT_OPEN - if repo_status is not PDS_REPO_OPEN
// PDS_REC_NOT_FOUND - if the key is not found
// PDS_SUCCESS - function completes without any error

// Purpose: retrieves a record by it's key.
// STEPS: 
// 1. Checks if the repo is open.
// 2. Rewinds the data file to start reading from the beginning.
// 3. Iterates through all recrods, comparing them using the matcher function.
// 4. Retursn the first matching record.
// Returns: PDS_SUCCESS
int get_rec_by_key( int key, void *rec );

//////////////////////////////////////////////////////////////////////////////
// get_rec_by_field
// Brute-force retrieval using an arbitrary search value
// 	search_count = 0
// 	fread key from data file until EOF
//	search_count++
//  fread the record
//  Invoke the matcher using current record and search value
//	if mathcher returns success, return the current record, else continue the loop
// end loop
//
// Return values:
//
// PDS_REPO_NOT_OPEN - if repo_status is not PDS_REPO_OPEN
// PDS_REC_NOT_FOUND - if the key is not found
// PDS_SUCCESS - function completes without any error
// comp_count should be the number of comparisons being done in the linear search
// 

// Purpose: Searches for a record based on a custom field .
// STEPS: 
// 1. CHecks if the repo is open.
// 2. Rewinds the data file to start reading from beginning.
// 3. Iterates through all records comparing them using the matcher function.
// 4. Returns the first matching record.
// Returns:
// PDS_SUCCESS

int get_rec_by_field(void *searchvalue, void *rec, int (*matcher)(void *rec, void *searchvalue), int *comp_count);

//////////////////////////////////////////////////////////////////////////////
// pds_close
// Open the index file in wb mode (write mode, not append mode)
// Function to use: fopen
// Store the number of records
// Function to use: fwrite
// [CHANGE] Unload the index into the index file by traversing the BST in pre-order mode (overwrite the entire index file)
// See function bst_print in bst.c to see how to do tree traversal
// Function to use: fwrite
// Free the BST and set pointer to NULL
// Function to use: bst_free
// Close the index file and data file
// Function to use: fclose
//
// Return values:
//
// [CHANGE] PDS_REPO_NOT_OPEN - if repo_status is not PDS_REPO_OPEN or if repo_name does not match with name stored in repo_info
// PDS_NDX_SAVE_FAILED - if fopen or fwrite fails
// PDS_SUCCESS - function completes without any error
int pds_close( char * repo_name );


#endif
