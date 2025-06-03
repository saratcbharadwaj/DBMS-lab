#include<stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
/////////////////////////////////////////////////////////////////////////
// class :
// Refactoring 
// ONLY DDL in this lab. Take care of ddl.
// Modify the database and organize it.
// PDS DBINFO structure :-> DB name, 
// num_tables and array of tables are in-memory structure.
// DBInfo -> DB is a collection of tables.
// DB handle-> 
// DB is going to be a folder.
// Create an empty folder instead of an empty file this time.
// SImply create a file with the name of the repo and do it.
// Create a folder 
// OPen db looks for presence of folder, and checks if folder exists.
// No new work as of now.
// helper function is also present.
// Previous repo functions are converted into new table functions such as->
// PDS_create_repo to PDS_create_folder.

// Still, nothing changed as of now. 
// Populating structure is also necessary.
// pds_Load_ndx -> load tables in this function. Helper function will simply take the name of the table and then work on that particular array. 
// Every function previously written should also be written. 
//  C functions learnt till now ( atleast half of them's known )
// mkdir is AKAZA. MKDIR function is the new major change 
// use mkdir in create function.
// change directory using chdir. 
// what the FUCK is going on? Why declare command lines in terminal as some seperate functions? 
// Is it concerned with different operating systems?
// FOUR functions: -> create db, open db, modify old 
//////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// Usage: ./a.out folder1 folder2
// This program will create folder1 and another folder inside folder1 called folder2
//

////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[])
{
	char *folder1 = argv[1];
	char *folder2 = argv[2];

	if(argc != 3){
		printf("Usage: %s <folder1> <folder2>\n", argv[0]);
		return 1;
	}
	// Create a folder in the current directory as given in the command line argument
	int mstatus = mkdir(folder1, 0777);
	if(mstatus == -1){
		perror("ERROR");
	}
	else{
		printf("Folder %s created\n", folder1);
	}

	// Change directory to the newly created folder
	int cstatus = chdir(folder1);
	if(cstatus == -1){
		perror("ERROR");
	}
	else{
		printf("Folder changed to %s\n", folder1);
	}

	// Create folder2 inside folder1
	mstatus = mkdir(folder2, 0777);
	if(mstatus == -1){
		perror("ERROR");
	}
	else{
		printf("Folder %s created\n", folder2);
	}
	return 0;
}

	
