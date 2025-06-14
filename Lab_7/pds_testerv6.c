#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "pdsv6.h"
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
	char command[30], param1[30], param2[30], param3[30], param4[30];
	char dbname[30], table_name[30], search_string[30], info[1024];
	int contact_id, rec_size_contact;
	int student_id, rec_size_student;
    int pds_status, expected_status;

	struct Contact testContact;
	struct Student testStudent;

	strcpy(testContact.contact_name, "dummy contact name");
	strcpy(testContact.phone, "dummy contact number");
	rec_size_contact = sizeof(struct Contact);

	strcpy(testStudent.student_name, "dummy student name");
	strcpy(testStudent.degree_name, "dummy degree name");
	rec_size_student = sizeof(struct Student);

	printf("Test case: %s", test_case); fflush(stdout);
	sscanf(test_case, "%d%s%s%s%s", &expected_status, command, param1, param2, param3);
	if( !strcmp(command,"CREATEDB") )
	{
		strcpy(dbname, param1);

		pds_status = pds_create_db( dbname );

		if( pds_status == expected_status ){
			TREPORT("PASS", "");
		}
		else{
			sprintf(info,"pds_create_db returned status %d",pds_status);
			TREPORT("FAIL", info);
		}
	}
	else if( !strcmp(command,"CREATETABLE") )
	{
		strcpy(table_name, param1);
	
		pds_status = pds_create_table( table_name );

		if( pds_status == expected_status ){
			TREPORT("PASS", "");
		}
		else{
			sprintf(info,"pds_create returned status %d",pds_status);
			TREPORT("FAIL", info);
		}
	}
	else if( !strcmp(command,"OPENDB") )
	{
		strcpy(dbname, param1);

		pds_status = pds_open_db( dbname );

		if( pds_status == expected_status ){
			TREPORT("PASS", "");
		}
		else{
			sprintf(info,"pds_open_db returned status %d",pds_status);
			TREPORT("FAIL", info);
		}
	}
	else if( !strcmp(command,"OPENTABLE") )
	{
		sscanf(param1, "%s", table_name);
		int rec_size;

		if(!strcmp(table_name, "contact"))
			rec_size = rec_size_contact;
		else
			rec_size = rec_size_student;

		pds_status = pds_open_table( table_name, rec_size );

		if( pds_status == expected_status ){
			TREPORT("PASS", "");
		}
		else{
			sprintf(info,"pds_open_table %s returned status %d",table_name, pds_status);
			TREPORT("FAIL", info);
		}
	}
	else if( !strcmp(command,"INSERT") )
	{
		sscanf(param1, "%s", table_name);
		if( !strcmp(table_name, "contact")){
			sscanf(param2, "%d", &contact_id);
			testContact.contact_id = contact_id;
			sprintf(testContact.contact_name, "Name-of-Contact-%d",contact_id);
			sprintf(testContact.phone, "Phone-of-Contact-%d",contact_id);		
			pds_status = put_rec_by_key( table_name, contact_id, &testContact );
		}
		else if( !strcmp(table_name, "student")){
			sscanf(param2, "%d", &student_id);
			testStudent.student_id = student_id;
			sprintf(testStudent.student_name, "Name-of-Student-%d",student_id);
			sprintf(testStudent.degree_name, "Degree-of-Student-%d",student_id);		
			pds_status = put_rec_by_key( table_name, student_id, &testStudent );
		}
		if( pds_status == expected_status ){
			TREPORT("PASS", "");
		}
		else{
			sprintf(info,"put_rec_by_key returned status %d", pds_status);
			TREPORT("FAIL", info);
		}
	}
	else if( !strcmp(command,"SELECT_BY_KEY") )
	{
		sscanf(param1, "%s", table_name);
		if( !strcmp(table_name, "contact")){
			sscanf(param2, "%d", &contact_id);
			testContact.contact_id = -1;

			pds_status = get_rec_by_key( table_name, contact_id, &testContact );

			if( pds_status != expected_status ){
				sprintf(info,"search key: %d; Got status %d",contact_id, pds_status);
				TREPORT("FAIL", info);
			}
			else{
				// Check if the retrieved values match
				char expected_contact_name[30];
				char expected_contact_phone[30];
				sprintf(expected_contact_name, "Name-of-Contact-%d", contact_id);
				sprintf(expected_contact_phone, "Phone-of-Contact-%d", contact_id);
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
		else if( !strcmp(table_name, "student")){
			sscanf(param2, "%d", &student_id);
			testStudent.student_id = -1;

			pds_status = get_rec_by_key( table_name, student_id, &testStudent );

			if( pds_status != expected_status ){
				sprintf(info,"search key: %d; Got status %d",student_id, pds_status);
				TREPORT("FAIL", info);
			}
			else{
				// Check if the retrieved values match
				char expected_student_name[30];
				char expected_student_degree[30];
				sprintf(expected_student_name, "Name-of-Student-%d", student_id);
				sprintf(expected_student_degree, "Degree-of-Student-%d", student_id);
				if( expected_status == PDS_SUCCESS ){
					if (testStudent.student_id == student_id && 
						strcmp(testStudent.student_name,expected_student_name) == 0 &&
						strcmp(testStudent.degree_name,expected_student_degree) == 0){
							TREPORT("PASS", "");
					}
					else{
						sprintf(info,"Student data not matching... Expected:{%d,%s,%s} Got:{%d,%s,%s}\n",
							student_id, expected_student_name, expected_student_degree, 
							testStudent.student_id, testStudent.student_name, testStudent.degree_name
						);
						TREPORT("FAIL", info);
					}
				}
				else
					TREPORT("PASS", "");
			}
		}
	}
	else if( !strcmp(command,"CONTACT-NAME-SEARCH") )
	{
		char contact_name[30], expected_name[30], expected_phone[30];
		int expected_comp_count, actual_comp_count;
		
		strcpy(table_name, "contact");
		sscanf(param1, "%s", contact_name);
		sscanf(param2, "%d", &expected_comp_count);
		testContact.contact_id = -1;
		actual_comp_count = -1;

		pds_status = get_rec_by_field( table_name, contact_name, &testContact, contact_name_matcher, &actual_comp_count );
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
	else if( !strcmp(command,"DELETE_BY_KEY") ){
		int delete_key;
		sscanf(param1, "%s", table_name);
		sscanf(param2, "%d", &delete_key);
		pds_status = delete_rec_by_key( table_name, delete_key );
		if( pds_status != expected_status ){
				sprintf(info,"delete key: %d; Got status %d",contact_id, pds_status);
				TREPORT("FAIL", info);
		}
		else{
				TREPORT("PASS", "");
		}
	}	
	else if( !strcmp(command,"CLOSETABLE") )
	{
		sscanf(param1, "%s", table_name);

		pds_status = pds_close_table( table_name );

		if( pds_status == expected_status ){
			TREPORT("PASS", "");
		}
		else{
			sprintf(info,"pds_close_table returned status %d",pds_status);
			TREPORT("FAIL", info);
		}
	}
	else if( !strcmp(command,"CLOSEDB") )
	{
		sscanf(param1, "%s", dbname);

		pds_status = pds_close_db( dbname );

		if( pds_status == expected_status ){
			TREPORT("PASS", "");
		}
		else{
			sprintf(info,"pds_close_db returned status %d",pds_status);
			TREPORT("FAIL", info);
		}
	}
}


