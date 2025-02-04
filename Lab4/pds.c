#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pdsv3.h"


struct PDS_RepoInfo repo_handle;


int pds_create(char *repo_name){

    	char file_name[30];
    	strcpy(file_name, repo_name);
    	strcat(file_name, ".dat");

	char index_file[30];
	strcpy(index_file,repo_name);
	strcat(index_file, ".ndx");
	
	repo_handle.pds_data_fp = fopen(file_name, "wb");
        repo_handle.pds_ndx_fp = fopen(index_file, "wb");
	
	int z=0;
	
	fwrite(&z, sizeof(int), 1, repo_handle.pds_ndx_fp);
	fclose( repo_handle.pds_data_fp );
	fclose( repo_handle.pds_ndx_fp );
}



int pds_open( char *repo_name, int rec_size ) {
	
    char file_name[30];
    char index_file[30];
	
    strcpy(file_name, repo_name);
    strcat(file_name, ".dat");
	
    strcpy(index_file, repo_name);
    strcat(index_file, ".ndx");

    if ( repo_handle.repo_status == PDS_REPO_OPEN) {
        
        return PDS_REPO_ALREADY_OPEN;
    }


    repo_handle.pds_data_fp = fopen(file_name, "rb+");
    repo_handle.pds_ndx_fp = fopen(index_file, "rb+");
    
    if (repo_handle.pds_data_fp != NULL) {
        
        strcpy(repo_handle.pds_name, repo_name);
        
        repo_handle.repo_status = PDS_REPO_OPEN;
        repo_handle.rec_size = rec_size;
	
	fread(&repo_handle.rec_count,sizeof(int),1,repo_handle.pds_ndx_fp);
	fread(repo_handle.ndx_array,sizeof(struct PDS_NdxInfo),repo_handle.rec_size,repo_handle.pds_ndx_fp);
        
        return PDS_SUCCESS;
    } 
    
    else {
        
        return PDS_FILE_ERROR;
    }
    
    fclose(repo_handle.pds_ndx_fp);
}
// put_rec_by_key
// Seek to the end of the data file
// Create an index entry with the current data file location using ftell
// Add index entry to ndx_array using offset returned by ftell
// Increment record count
// Return failure in case of duplicate key

int put_rec_by_key(int key, void *rec) {
    
    if (repo_handle.repo_status != PDS_REPO_OPEN) {
        
        return PDS_FILE_ERROR;
    }


    fseek(repo_handle.pds_data_fp, 0, SEEK_END);
    int found =0;
    
    for ( int i = 0; i < repo_handle.rec_count; i++) {
	    
	    if ( repo_handle.ndx_array[i].key == key ) {
			    found=1;
			    break;
	        }
    }

    if ( found ) {
	    
	    return PDS_ADD_FAILED;
    }

    if ( fseek(repo_handle.pds_data_fp, 0, SEEK_END) != 0 ) {
	    
	    return PDS_FILE_ERROR;
    }

    int offset = ftell(repo_handle.pds_data_fp);
    
    if ( fwrite(rec, repo_handle.rec_size,1,repo_handle.pds_data_fp) != 1 ) {
	    
	    return PDS_FILE_ERROR;
    }
    
    repo_handle.ndx_array[repo_handle.rec_count].key = key;
    repo_handle.ndx_array[repo_handle.rec_count].offset = offset;

    repo_handle.rec_count++;

    return PDS_SUCCESS;
}

int get_rec_by_key(int key, void *rec) {
    
    if ( repo_handle.repo_status != PDS_REPO_OPEN ) {
	    
	    return PDS_REPO_NOT_OPEN;
    }

    if ( repo_handle.pds_data_fp == NULL || repo_handle.pds_ndx_fp == NULL ) {
	    
	    return PDS_FILE_ERROR;
    }

    int found=0;
    int found_index=0;
    
    for ( int i=0; i<repo_handle.rec_count; i++){
	    
	    if ( key == repo_handle.ndx_array[i].key ) {
		found =1;
		found_index = i;
		break;
	    }
    }
    
    if ( !found ) {
	    
	    return PDS_REC_NOT_FOUND;
    }
    
    int offset = repo_handle.ndx_array[found_index].offset;
	

    fseek( repo_handle.pds_data_fp,offset, SEEK_SET);
    
    if ( fread(rec ,repo_handle.rec_size,1,repo_handle.pds_data_fp) != 1 ) {
	    
	    return PDS_REC_NOT_FOUND;
    }
    
    return PDS_SUCCESS;
}



int pds_close(char *repo_name ){
	
	if ( strcmp(repo_name,repo_handle.pds_name) != 0 ) {
	    return PDS_FILE_ERROR;
	}
			
	char index_file[30];
        strcpy(index_file,repo_name);
        strcat(index_file, ".ndx");
		
    	if ( repo_handle.repo_status == PDS_REPO_CLOSED ) {
            
            return PDS_REPO_CLOSED;
    	}
    	
    	repo_handle.pds_ndx_fp = fopen(index_file,"wb");
    	
    	if ( repo_handle.pds_ndx_fp == NULL ) {
    			
    	    return PDS_FILE_ERROR;
    	}
    	
    	fwrite( &repo_handle.rec_count, sizeof(int), 1, repo_handle.pds_ndx_fp);
    	
    	fwrite( repo_handle.ndx_array, sizeof(struct PDS_NdxInfo), repo_handle.rec_count, repo_handle.pds_ndx_fp);

    	if ( fclose( repo_handle.pds_data_fp ) != 0 ) {
        		
            return PDS_FILE_ERROR;
    	}
    	
    	if ( fclose( repo_handle.pds_ndx_fp ) != 0 ) {
    			
    	    return PDS_FILE_ERROR;
        }

    	repo_handle.repo_status = PDS_REPO_CLOSED;
    

    return PDS_SUCCESS;
}


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
//int get_rec_by_field(void *searchvalue, void *rec, int (*matcher)(void *rec, void *searchvalue), int *comp_count) {

//if (repo_handle.repo_status != PDS_REPO_OPEN) {
 // return PDS_REPO_NOT_OPEN;
//}

//int search_count = 0;

//while (search_count < repo_handle.rec_count) {

//if ( fread(rec ,repo_handle.rec_size,1,repo_handle.pds_data_fp) != 1 ) {
	    
	//    return PDS_REC_NOT_FOUND;
   // }
//if (matcher(rec, &search_count) != -1) {

int get_rec_by_field(void *searchvalue, void *rec, int (*matcher)(void *rec, void *searchvalue), int *comp_count) {
    if (repo_handle.repo_status != PDS_REPO_OPEN) {
        return PDS_REPO_NOT_OPEN;
    }

    fseek(repo_handle.pds_data_fp, 0, SEEK_SET);
    *comp_count = 0;

    while (!feof(repo_handle.pds_data_fp)) {
        if (fread(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp) != 1) {
            if (feof(repo_handle.pds_data_fp)) {
                break;
            }
            return PDS_FILE_ERROR;
        }

        (*comp_count)++;

        if (matcher(rec, searchvalue) == 0) {
            return PDS_SUCCESS;
        }
    }

    return PDS_REC_NOT_FOUND;
}







