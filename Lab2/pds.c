#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pds.h"

struct PDS_RepoInfo repo_handle;

void set_up_details() {

  repo_handle.pds_data_fp = NULL;
  repo_handle.repo_status = PDS_REPO_CLOSED;
  repo_handle.rec_size = 0;
  memset(repo_handle.pds_name, 0, sizeof(repo_handle.pds_name));
  
}

int pds_open(char *repo_name, int rec_size) {

    set_up_details();
    
    char filename[30];
    strcpy(filename, repo_name);
    strcat(filename, ".dat");
    
    repo_handle.pds_data_fp = fopen(filename, "ab+");
    
    if (repo_handle.pds_data_fp == NULL) {
    
          return PDS_FILE_ERROR;      
    }
    
    strncpy(repo_handle.pds_name, repo_name, sizeof(repo_handle.pds_name) - 1);
    repo_handle.pds_name[sizeof(repo_handle.pds_name) - 1] = '\0';
    
    repo_handle.rec_size = rec_size;
    repo_handle.repo_status = PDS_REPO_OPEN;
    
    return PDS_SUCCESS;
}

int put_rec_by_key(int key, void *rec) {

  if (repo_handle.repo_status != PDS_REPO_OPEN || repo_handle.pds_data_fp == NULL) {

      return PDS_REPO_ALREADY_CLOSED;
  }

  if (fwrite(&key, sizeof(int), 1, repo_handle.pds_data_fp) != 1) {
  
      return PDS_ADD_FAILED;
  }
  
  if ( fwrite(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp) != 1) {

      return PDS_ADD_FAILED;
  }
  
  return PDS_SUCCESS;
}

int get_rec_by_key(int key, void *rec) {

    if (repo_handle.repo_status != PDS_REPO_OPEN || repo_handle.pds_data_fp == NULL) {
    
        return PDS_REPO_ALREADY_CLOSED;
    }
    
    int ckey;
    
    while (fread(&ckey, sizeof(int), 1, repo_handle.pds_data_fp) == 1) {
    
          if (fread(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp) != 1) {
          
              return PDS_FILE_ERROR;
          }
          
          if ( ckey == key) {
          
              return PDS_SUCCESS;
          }
    }
    
    return PDS_REC_NOT_FOUND;
}

int pds_close() {

    if (repo_handle.repo_status == PDS_REPO_CLOSED || repo_handle.pds_data_fp == NULL) {
    
        return PDS_REPO_ALREADY_CLOSED;
    }
    
    set_up_details();
    
    return PDS_SUCCESS;
    
}
          
