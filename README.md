# ------------> Persistent Data Store in C <------------

## Hello Reader!

This repository holds a good tutorial for building a persistent data-store from scratch, in C.

The repo holds questions and answers to the tasks assigned for the first part during the course ->  DAS 101P-Databse Systems Lab 
taught during Spring 2025 in IIIT-Bangalore.

Before proceeding to make use of this repo, I request you to ensure that you're comfortable with the following skills in C language:
1. File Handling
2. Exception Handling

The folders in the repo(Lab 1 till Lab 7) lays a strong foundation for understanding how database management systems work.
Here's an overview about what the course offers:


Lab-1 provides an introduction to data storage formats (binary and text) and fundamental operations, including capturing user input via the command line and retrieving data from both formats.

Lab-2 begins the pathway in bulding the datastore,

It kicks off by introducing fundamental operations such as 
1. Opening and closing a repository (datastore/database) 
2. storing and retrieving data with the help of primary key in the data fie.

It is followed by Lab-3 and Lab-4 which optimizes the above operations as follows:
1. Introduces an index file.
2. Loading keys into the index file while storing new data

Lab-5 further optimizes the search for a key in the index file by constructing a BST for keys stored in index file.
It is followed by Lab-6 which introduces another operation ->
1. Deleting record by key.
## Note : Lab-1 to Lab-6 involves operations including a single table (or a single relationship)
Lab-7 helps you to implement the operations in a real database(i.e multiple relationships )
