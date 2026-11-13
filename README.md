# Inverted-Search
   A command-line inverted search engine written in C. It reads a set of .txt files, builds an in-memory word database, and lets you find which files contain a word and how many times it appears in each. The database can be saved to a backup file and restored later.

Features:
* Create database from one or more .txt files given on the command line
* Display database in a formatted table (index, word, file count, per-file word counts)
* Search database for a word and see every file it appears in, with counts
* Save database to a backup .txt file
* Update database by loading a previously saved backup into an empty database
* Robust input handling: invalid, duplicate, missing, empty and backup files are filtered out before processing
  
Data Structures:
   The database is a hash table of 27 buckets, each holding a linked list of words, and each word holding a linked list of files.

Hash_table[27]
  index 0-25 -> words starting with a-z (case-insensitive)
  index 26   -> words starting with any other character
      |
      v
   Main node (word, file_count)  -->  Main node  -->  ...
      |
      v
   Sub node (file_name, word_count)  -->  Sub node  -->  ...
   
Structure	Purpose:
* File - Singly linked list of validated input file names
* Main - A unique word and the number of files it appears in
* Sub -	A file name and how many times the word occurs in that file
* Hash_table -	27 buckets indexed by the first letter of the word
  
Project Structure:
.
├── main.c       # Menu-driven driver
├── function.c   # Validation, database create/display/search/save/update
├── main.h       # Structures, macros, function prototypes
└── Makefile

Build and Run:
bash
gcc main.c function.c -o a.out
./a.out file1.txt file2.txt file3.txt

Or with the Makefile:
bash
make
./a.out file1.txt file2.txt

Menu
1. Create database
2. Display database
3. Search database
4. Save database
5. Update database
6. Exit

Sample output:
Index  Word            File Count  [File Name: Word Count]...
-------------------------------------------------------------------
[0  ]  apple            2           file1.txt  3      file2.txt  1
[1  ]  ball             1           file1.txt  2
-------------------------------------------------------------------
Enter the word to search : apple

Word apple is present in 2 files
In file1.txt : 3 times
In file2.txt : 1 times

Command-Line Validation:
Before the database is built, each argument is checked and rejected if it:
* does not have a .txt extension
* is a duplicate of an earlier argument
* does not exist in the current folder
* is empty
* is a backup file created by this program (identified by the #* header)

The program exits with a usage message if no valid file remains.

Backup File Format:
#*                                  <- magic string
#:<bucket index>                    <- bucket header
word:<file_count>:<file>:<count>:...:#

Example:
#* -> magic string
#:0
apple:2:file1.txt:3:file2.txt:1:#
#:1
ball:1:file1.txt:2:#

Design Notes:
* Create can be run only once per session, which prevents duplicate counts.
* Update is allowed only on an empty database and is all-or-nothing: if the backup is malformed, anything partly loaded is freed and the database stays empty.
* After a successful update, files already present in the backup are removed from the file list so they are not indexed again.
* Words are case-sensitive for matching, but bucket selection ignores case.
* Handles EOF and non-numeric input in the menu without crashing.
  
Limitations:
* Words are limited to 49 characters and file names to 49 characters.
* Words are split on whitespace, so punctuation attached to a word is treated as part of it.
  
Concepts Demonstrated:
* Linked lists (nested, three levels)
* Hashing with chained buckets
* Dynamic memory allocation and cleanup
* File I/O and parsing with fgets, fscanf, strtok
* Command-line argument validation
* Error handling with goto-based cleanup
