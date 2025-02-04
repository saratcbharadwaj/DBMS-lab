#ifndef PDS_H
#define PDS_H

// Error codes
#define PDS_SUCCESS 0
#define PDS_FILE_ERROR 1
#define PDS_ADD_FAILED 2
#define PDS_REC_NOT_FOUND 3
#define PDS_REPO_ALREADY_OPEN 4
#define PDS_REPO_ALREADY_CLOSED 5

// Repository status values
#define PDS_REPO_OPEN 10
#define PDS_REPO_CLOSED 11

#define ERROR_LOG(code, errormsg) printf("Error code: %d - %s", code, errormsg)

struct PDS_RepoInfo{
	char pds_name[30];
	FILE *pds_data_fp;
	int repo_status; 
	int rec_size; // For fixed length records
};

extern struct PDS_RepoInfo repo_handle;

// Open data files as per the following convention
// If repo_name is "demo", then data file should be "demo.dat"
// Initialize other members of PDS_RepoInfo global variable
// Open the file in "ab+" mode
// Append: open or create a file for update in binary mode; 
// File read/write location is placed at the end of the file
//////////////////////////////////////////////////////
// Functions to use: 
// fopen in binary append mode
//////////////////////////////////////////////////////
int pds_open( char *repo_name, int rec_size );

//////////////////////////////////////////////////////
// Store record in the data file
// Seek to the end of the data file using fseek with SEEK_END
// Write the <key,record> pair at the current file location
// Access the necessary fwrite parameters from repo_handle
//////////////////////////////////////////////////////
// Functions to use: Check sample code for usage
// fseek to end of file
// fwrite key
// fwrite record
//////////////////////////////////////////////////////
int put_rec_by_key( int key, void *rec );

//////////////////////////////////////////////////////
// Read the record based on the given key
// Seek to the beginning of the file using fseek with SEEK_SET
// Set up a loop to read <key, record> pair till the matching key is found
// Functions to use: Check sample code for usage
// fseek to beginning of file
// fread key
// fread record
//////////////////////////////////////////////////////
int get_rec_by_key( int key, void *rec );

// Close all files and reset repo_handle values 
// // Functions to use: Check sample code for usage
// fclose all file pointers
int pds_close();

#endif
