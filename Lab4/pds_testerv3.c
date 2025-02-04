#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "pdsv3.h"
#include "contact.h"

#define TREPORT(a1,a2) printf("Status: %s - %s\n\n",a1,a2); fflush(stdout);

void process_line( char *test_case );

// New function to match contact names
int contact_name_matcher(void *rec, void *search_name);

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
	char command[30], param1[30], param2[30], info[1024];
	int contact_id, rec_size;
    int pds_status, expected_status;

	struct Contact testContact;

	strcpy(testContact.contact_name, "dummy name");
	strcpy(testContact.phone, "dummy number");

	rec_size = sizeof(struct Contact);

	sscanf(test_case, "%s%s%d%s", command, param1, &expected_status, param2);
	printf("Test case: %s", test_case); fflush(stdout);
	if( !strcmp(command,"CREATE") )
	{
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
	else if( !strcmp(command,"OPEN") )
	{
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
	else if( !strcmp(command,"STORE") )
	{

		sscanf(param1, "%d", &contact_id);
		testContact.contact_id = contact_id;
		sprintf(testContact.contact_name, "Name-of-%d",contact_id);
		sprintf(testContact.phone, "Phone-of-%d",contact_id);		
		pds_status = put_rec_by_key( contact_id, &testContact );

		if( pds_status == expected_status ){
			TREPORT("PASS", "");
		}
		else{
			sprintf(info,"add_contact returned status %d", pds_status);
			TREPORT("FAIL", info);
		}
	}
	else if( !strcmp(command,"KEY_SEARCH") )
	{
		sscanf(param1, "%d", &contact_id);
		testContact.contact_id = -1;

		pds_status = get_rec_by_key( contact_id, &testContact );

		if( pds_status != expected_status ){
			sprintf(info,"search key: %d; Got status %d",contact_id, pds_status);
			TREPORT("FAIL", info);
		}
		else{
			// Check if the retrieved values match
			char expected_contact_name[30];
			char expected_contact_phone[30];
			sprintf(expected_contact_name, "Name-of-%d", contact_id);
			sprintf(expected_contact_phone, "Phone-of-%d", contact_id);
			if( expected_status == PDS_SUCCESS ){
				if (testContact.contact_id == contact_id && 
					strcmp(testContact.contact_name,expected_contact_name) == 0 &&
					strcmp(testContact.phone,expected_contact_phone) == 0){
						TREPORT("PASS", "");
				}
				else{
					sprintf(info,"Contact data not matching... Expected:{%d,%s,%s} Got:{%d,%s,%s}\n",
						contact_id, expected_contact_name, expected_contact_phone, 
						testContact.contact_id, testContact.contact_name, testContact.phone
					);
					TREPORT("FAIL", info);
				}
			}
			else
				TREPORT("PASS", "");
		}
	}
	else if( !strcmp(command,"CONTACT-NAME-SEARCH") )
	{
		char contact_name[30], expected_name[30], expected_phone[30];
		int expected_comp_count, actual_comp_count;
		
		sscanf(param1, "%s", contact_name);
		sscanf(param2, "%d", &expected_comp_count);
		testContact.contact_id = -1;
		actual_comp_count = -1;
		pds_status = get_rec_by_field( contact_name, &testContact, contact_name_matcher, &actual_comp_count );
		if( pds_status != expected_status )
		{
			sprintf(info,"search field: %s; Got status %d",contact_name, pds_status);
			TREPORT("FAIL", info);
		}
		else if( pds_status == PDS_REC_NOT_FOUND )
		{
			// Nothing else to check... all good
			TREPORT("PASS", "");
		}
		else {
			// Check if the retrieved values match
			// Check if num block accesses match too
			// Extract the expected contact_id from the phone number
			sscanf(contact_name+sizeof("Name-of"), "%d", &contact_id);
			sprintf(expected_name,"Name-of-%d",contact_id);
			sprintf(expected_phone,"Phone-of-%d",contact_id);
			if (testContact.contact_id == contact_id && 
				strcmp(testContact.contact_name, expected_name) == 0 &&
				strcmp(testContact.phone, expected_phone) == 0 )
			{
				if( expected_comp_count == actual_comp_count ){
					TREPORT("PASS", "");
				}
				else{
					sprintf(info,"Search count not matching for contact %d... Expected:%d Got:%d\n",
						contact_id, expected_comp_count, actual_comp_count
					);
					TREPORT("FAIL", info);
				}
			}
			else{
				sprintf(info,"Contact data not matching... Expected:{%d,%s,%s} Got:{%d,%s,%s}\n",
					contact_id, expected_name, expected_phone, 
					testContact.contact_id, testContact.contact_name, testContact.phone
				);
				TREPORT("FAIL", info);
			}
		}
	}
	else if( !strcmp(command,"CLOSE") )
	{
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


