#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "pds.h"
#include "contact.h"

#define TREPORT(a1,a2) printf("Status: %s - %s\n\n",a1,a2); fflush(stdout);

void process_line( char *test_case );

int main(int argc, char *argv[])
{
	FILE *cfptr;
	char test_case[50];

	if( argc != 2 ){
		fprintf(stderr, "Usage: %s testcasefile\n", argv[0]);
		exit(1);
	}

	cfptr = (FILE *) fopen(argv[1], "r");
	while(fgets(test_case, sizeof(test_case)-1, cfptr)){
		// printf("line:%s",test_case);
		if( !strcmp(test_case,"\n") || !strcmp(test_case,"") )
			continue;
		process_line( test_case );
	}
}

void process_line( char *test_case )
{
	char repo_name[30];
	char command[10], param1[10], param2[10], info[1024];
	int contact_id, rec_size;
    int pds_status, expected_status;

	struct Contact testContact;

	strcpy(testContact.contact_name, "dummy name");
	strcpy(testContact.phone, "dummy number");

	rec_size = sizeof(struct Contact);

	sscanf(test_case, "%s%s%d", command, param1, &expected_status);
	printf("Test case: %s", test_case); fflush(stdout);
	if( !strcmp(command,"CREATE") ){
		strcpy(repo_name, param1);

		pds_status = pds_create( repo_name );

		if( pds_status == expected_status ){
			TREPORT("PASS", "");
		}
		else{
			sprintf(info,"pds_create returned status %d",pds_status);
			TREPORT("FAIL", info);
		}
	}
	else if( !strcmp(command,"OPEN") ){
		strcpy(repo_name, param1);

		pds_status = pds_open( repo_name, rec_size );

		if( pds_status == expected_status ){
			TREPORT("PASS", "");
		}
		else{
			sprintf(info,"pds_open returned status %d",pds_status);
			TREPORT("FAIL", info);
		}
	}
	else if( !strcmp(command,"STORE") ){

		sscanf(param1, "%d", &contact_id);
		testContact.contact_id = contact_id;
		pds_status = put_rec_by_key( contact_id, &testContact );

		if( pds_status == expected_status ){
			TREPORT("PASS", "");
		}
		else{
			sprintf(info,"add_contact returned status %d", pds_status);
			TREPORT("FAIL", info);
		}
	}
	else if( !strcmp(command,"SEARCH") ){
		sscanf(param1, "%d", &contact_id);
		testContact.contact_id = -1;

		pds_status = get_rec_by_key( contact_id, &testContact );

		if( pds_status != expected_status ){
			sprintf(info,"search key: %d; Got status %d",contact_id, pds_status);
			TREPORT("FAIL", info);
		}
		else{
			// Check if the retrieved values match
			if( expected_status == PDS_SUCCESS ){
				if (testContact.contact_id == contact_id && 
					strcmp(testContact.contact_name,"dummy name") == 0 &&
					strcmp(testContact.phone,"dummy number") == 0){
						TREPORT("PASS", "");
				}
				else{
					sprintf(info,"Contact data not matching... Expected:{%d,%s,%s} Got:{%d,%s,%s}\n",
						contact_id, "dummy_name", "dummy number", 
						testContact.contact_id, testContact.contact_name, testContact.phone
					);
					TREPORT("FAIL", info);
				}
			}
			else
				TREPORT("PASS", "");
		}
	}
	else if( !strcmp(command,"CLOSE") ){
		strcpy(repo_name, param1);

		pds_status = pds_close( repo_name );

		if( pds_status == expected_status ){
			TREPORT("PASS", "");
		}
		else{
			sprintf(info,"pds_close returned status %d",pds_status);
			TREPORT("FAIL", info);
		}
	}
}


