#ifndef PDS_H
#define PDS_H

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
	struct PDS_NdxInfo ndx_array[MAX_NDX_SIZE];
};

extern struct PDS_RepoInfo repo_handle;

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
int pds_open( char *repo_name, int rec_size );

// pds_load_ndx
// Internal function used by pds_open to read index entries into ndx_array
// Read the number of records form the index file
// Function to use: fread
// Load the index into the array and store in ndx_array by reading index entries from the index file
// // Function to use: fread
//
// Return values:
//
// PDS_FILE_ERROR - if fread returns less than num_records
// PDS_SUCCESS - function completes without any error
int pds_load_ndx();

// put_rec_by_key
// Seek to the end of the data file
// Function to use: fseek
// Identify the current location of file pointer
// Function to use: ftell
// Create an index entry with the current data file location using ftell
// Add index entry to ndx_array using offset returned by ftell
// Increment record count
// Write the record at the end of the data file
// Function to use: fwrite
//
// Return values:
//
// PDS_REPO_NOT_OPEN - if repo_status is not PDS_REPO_OPEN
// PDS_ADD_FAILED - if fwrite fails OR duplicate record is found
// PDS_SUCCESS - function completes without any error
int put_rec_by_key( int key, void *rec );

// get_rec_by_key
// Search for index entry in ndx_array
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
int get_rec_by_key( int key, void *rec );

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
int get_rec_by_field(void *searchvalue, void *rec, int (*matcher)(void *rec, void *searchvalue), int *comp_count);

// pds_close
// Open the index file in wb mode (write mode, not append mode)
// Function to use: fopen
// Store the number of records
// Function to use: fwrite
// Unload the ndx_array into the index file (overwrite the entire index file)
// Function to use: fwrite
// Close the index file and data file
// Function to use: fclose
//
// Return values:
//
// PDS_NDX_SAVE_FAILED - if fopen or fwrite fails
// PDS_SUCCESS - function completes without any error
int pds_close( char * repo_name );


#endif
