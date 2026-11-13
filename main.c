#include "main.h"

int main(int argc, char* argv[])
{
    File*      f_head = NULL;
    Hash_table ht[27];

    // Initialize the hash table pointers to NULL
    for (int i = 0; i < 27; i++)
    {
        ht[i].link = NULL;
    }

    if (cla_validation(argc, argv) == FAILURE)
    {
        printf("Usage : ./a.out file1.txt\n");
        return 1;
    }

    if (save_file(argc, argv, &f_head) == FAILURE)
    {
        printf("File list creation failed\n");
        return 1;
    }


    int flag = 0;
    // Display menu
    do
    {
        printf("\nDisplay Menu :\n");
        printf("1. Create database\n");
        printf("2. Display database\n");
        printf("3. Search database\n");
        printf("4. Save database\n");
        printf("5. Update database\n");
        printf("6. Exit\n");

        int op;
        printf("Enter your option : ");
        int r = scanf("%d", &op);
        if (r == EOF)
        {
            printf("\nInput closed. Exiting.\n");
            break;  // leaves the main do-while
        }
        if (r != 1)
        {
            printf("Invalid input\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        switch (op)
        {
            // 1. Create database
            case 1:
            {
                if (flag)
                {
                    printf("\nError : Create database should be performed only once\n");
                }
                else if (create_database(&f_head, ht) == FAILURE)
                    printf("Error : Can't Create database\n");
                else
                    flag = 1;
            }
            break;

            // 2. Display database
            case 2:
            {
                if(display_database(ht) == FAILURE)
                {
                    printf("\nEmpty Database\n");
                }
            }
            break;

            // 3. Search database
            case 3:
            {
                char word[100];

                printf("Enter the word to search : ");
                scanf(" %99s", word);

                if (search_database(ht, word) == FAILURE)
                {
                    printf("\nWord is not present in the database\n");
                }
            }
            break;

            // 4. Save database
            case 4:
            {
                char f_name[100];

                printf("Enter the filename to save database : ");
                scanf(" %99s", f_name);

                if (save_database(f_name, ht) == FAILURE)
                {
                    printf("\nError : Couldn't open the file\n");
                }
                else
                    printf("\nSuccessfully Saved the database\n");
            }
            break;

            // 5. Update database
            case 5:
            {
                char f_name[100];

                printf("Enter the backup filename to update database : ");
                scanf(" %99s", f_name);

                if (update_database(f_name, ht, &f_head) == FAILURE)
                    printf("\nError : Invalid backup file, or database is not empty\n");
                else
                    printf("\nDatabase updated successfully\n");
            }
            break;

            case 6:
                return 0;

            default:
                printf("Invalid input\n");
                break;
        }

        int y = 0, n = 0;
        do
        {
            char ch;

            printf("\nDo you want to continue?\n");
            printf("Enter y/Y to continue and n/N to discontinue\n");
            // EOF: treat as "no"
            if (scanf(" %c", &ch) != 1)
            {
                n = 1;
                break;
            }

            if (ch == 'y' || ch == 'Y')
            {
                y = 1;
                break;
            }
            else if (ch == 'n' || ch == 'N')
            {
                n = 1;
                break;
            }
            else
            {
                printf("Invalid input\n");
                continue;
            }

        } while (1);

        if(y)
           continue;
        else if(n)
           break;



    } while (1);

    return 0;
}
