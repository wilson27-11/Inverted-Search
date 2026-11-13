#include  "main.h"

// Validate the command line arguments
int cla_validation(int argc, char* argv[])
{
    // Check input count
    if (argc < 2) return FAILURE;

    // Check extension
    int i, j, count = 0;

    for (i = 1; i < argc; i++)
    {
        char* ptr = strrchr(argv[i], '.');
        if (ptr == NULL || (strcmp(ptr, ".txt") != 0))
        {
            argv[i] = "-1";
        }
        else
            count++;
    }

    // Check the duplicate file
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-1") == 0) continue;

        for (j = i + 1; j < argc; j++)
        {
            if ((strcmp(argv[j], "-1") != 0) && (strcmp(argv[i], argv[j]) == 0))
            {
                // Mark as invalid
                argv[j] = "-1";
                count--;
            }
        }
    }

    // Check the file in same folder and file empty
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-1") == 0) continue;

        FILE* fp = fopen(argv[i], "r");

        // File is not available
        if(fp == NULL)
        {
            argv[i] = "-1";
            count--;
            continue;
        }

        // File empty
        fseek(fp, 0, SEEK_END);
        if (ftell(fp) == 0)
        {
            argv[i] = "-1";
            count--;
            fclose(fp);
            continue;  // already rejected, skip the backup check
        }

        // Backup file cannot be an input file
        // go back to the start after fseek
        rewind(fp);
        char line[4] = "";
        if (fscanf(fp, "%3s", line) == 1 && strcmp(line, MAGIC_STRING) == 0)
        {
            argv[i] = "-1";
            count--;
        }

        fclose(fp);
    }

    // Check the valid input count
    if (count < 1) return FAILURE;

    return SUCCESS;
}

// Insert at last for file name list
int insert_last_file(File** head, char* str)
{
    // Allocate memory
    File* new = malloc(sizeof(File));

    // Validate
    if (new == NULL) return FAILURE;

    // Validate filename length
    if (strlen(str) >= sizeof(new->f_name))
    {
        free(new);
        return FAILURE;
    }

    // Update
    strcpy(new->f_name, str);
    new->link = NULL;

    // list empty
    if (*head == NULL)
    {
        *head = new;

        return SUCCESS;
    }

    // List not empty
    File* temp = *head;

    while(temp->link != NULL)
    {
        temp = temp->link;
    }

    temp->link = new;

    return SUCCESS;
}

// Insert at last for Main list
int insert_last_main(Main** head, char* word, char* f_name)
{
    Main* temp = *head;

    // Check word already present
    while (temp != NULL)
    {
        if (strcmp(temp->word, word) == 0)
        {
            Sub* s = temp->sub_link;
            int  file_flag = 0;

            while(s != NULL)
            {
                if(strcmp(s->f_name, f_name) == 0)
                {
                    file_flag = 1;
                    break;
                }
                s = s->link;
            }

            if(!file_flag)
                temp->f_count++;

            return insert_last_sub(&temp->sub_link, f_name);
        }
        temp = temp->link;
    }

    // Word not found, create new main node
    // Allocate memory
    Main* new = malloc(sizeof(Main));

    // Validate
    if (new == NULL) return FAILURE;

    // Update
    strcpy(new->word, word);
    new->f_count = 1;
    new->link = NULL;
    new->sub_link = NULL;


    // Insert the file into sub list
    if (insert_last_sub(&new->sub_link, f_name) == FAILURE)
    {
        free(new);
        return FAILURE;
    }


    if(*head == NULL)
    {
        *head = new;
    }
    else
    {
        temp = *head;
        while (temp->link != NULL)
        {
            temp = temp->link;
        }

        temp->link = new;
    }

    return SUCCESS;
}

// Insert at last for Sub list
int insert_last_sub(Sub** head, char* f_name)
{
    Sub* temp = *head;

    // Check file already present
    while (temp != NULL)
    {
        if (strcmp(temp->f_name, f_name) == 0)
        {
            temp->word_count++;
            return SUCCESS;
        }
        temp = temp->link;
    }

    // Create new sub node
    Sub* new = malloc(sizeof(Sub));

    // Validate
    if(new == NULL)
        return FAILURE;

    // Update
    strcpy(new->f_name, f_name);
    new->word_count = 1;
    new->link       = NULL;

    // List empty
    if((*head) == NULL)
    {
        *head = new;
    }
    else
    {
        temp = *head;
        while(temp->link != NULL)
        {
            temp = temp->link;
        }

        temp->link = new;
    }

    return SUCCESS;
}

// Save file name in list
int save_file(int argc, char* argv[], File** f)
{
    int i;

    for (i = 1; i < argc; i++)
    {
        if(strcmp(argv[i], "-1") != 0)
        {
            if(insert_last_file(f, argv[i]) == FAILURE)
            {
                return FAILURE;
            }
        }
    }
    return SUCCESS;
}

// Print list
int print_list(File* head)
{
    int i = 1;

    if (head == NULL) return FAILURE;

    while (head != NULL)
    {
        printf("%d. %s\n", i++, head->f_name);
        head = head->link;
    }

    return SUCCESS;
}

// Create database
int create_database(File** head, Hash_table* ht)
{
    File* temp = *head;

    while(temp != NULL)
    {
        FILE* fp = fopen(temp->f_name, "r");
        char  buf[50];

        if(fp == NULL)
        {
            return FAILURE;
        }

        while (fscanf(fp, "%49s", buf) == 1)
        {
            //printf("%s\n", buf);
            int index;

            if(buf[0] >= 'a' && buf[0] <= 'z')
            {
                index = buf[0] - 'a';
            }
            else if (buf[0] >= 'A' && buf[0] <= 'Z')
            {
                index = buf[0] - 'A';
            }
            else
            {
                index = 26;
            }

            // Create database
            ht[index].index = index;
            insert_last_main(&ht[index].link, buf, temp->f_name);
        }

        fclose(fp);

        temp = temp->link;
    }

    printf("\nDatabase created successfully\n");
    return SUCCESS;
}

// Display database
int display_database(Hash_table* ht)
{
    int i, flag = 1;

    for (int k = 0; k < 27; k++)
    {
        if (ht[k].link != NULL)
        {
            flag = 0;
            break;
        }
    }

    if (flag) return FAILURE;

    printf("\n%-6s %-15s %-12s", "Index", "Word", "File Count");
    // Print dynamic columns or a generic header for sub-nodes
    printf("  [File Name: Word Count]...\n");
    printf("-------------------------------------------------------------------\n");

    for (i = 0; i < 27; i++)
    {
        if (ht[i].link == NULL)
        {
            continue;
        }

        Main* m = ht[i].link;

        while (m != NULL)
        {
            // Print Index, Word, and File Count first
            printf("[%-3d]  %-15s  %-12d", i, m->word, m->f_count);

            // Traverse the sub-list and print each file horizontally on the same line
            Sub* s = m->sub_link;
            while (s != NULL)
            {
                printf("  %-12s  %-5d", s->f_name, s->word_count);
                s = s->link;
            }

            // Move to the next line for the next main word
            printf("\n");

            m = m->link;
        }
    }
    printf("-------------------------------------------------------------------\n");
    return SUCCESS;
}

// Search database
int search_database(Hash_table* ht, char* word)
{
    int index;

    if (word[0] >= 'a' && word[0] <= 'z')
    {
        index = word[0] - 'a';
    }
    else if (word[0] >= 'A' && word[0] <= 'Z')
    {
        index = word[0] - 'A';
    }
    else
    {
        index = 26;
    }

    Main* temp = ht[index].link;
    Sub*  s;

    if (temp == NULL) return FAILURE;

    int file_count = 0, i = 0, flag = 1;

    while(temp != NULL)
    {
        if(strcmp(temp->word, word) == 0)
        {
            file_count = temp->f_count;
            s       = temp->sub_link;
            flag       = 0;

            break;
        }
        temp = temp->link;
    }

    if (flag) return FAILURE;

    int  word_count[file_count];
    char file_name[file_count][100];

    while (s != NULL)
    {
        word_count[i] = s->word_count;
        strcpy(file_name[i++], s->f_name);

        s = s->link;
    }

    printf("\nWord %s is present in %d files\n", word, file_count);
    for (int j = 0; j < file_count; j++)
    {
        printf("In %s : %d times\n", file_name[j], word_count[j]);
    }

    return SUCCESS;
}

// Save database
int save_database(char* f_name, Hash_table* ht)
{
    // Check the file extension
    char* ptr = strrchr(f_name, '.');

    if (ptr == NULL || (strcmp(ptr, ".txt") != 0))
        return FAILURE;

    FILE* fp = fopen(f_name, "w");

    if (fp == NULL) return FAILURE;

    // Magic string
    fprintf(fp, "#*\n");

    for (int i = 0; i < 27; i++)
    {
        Main* temp = ht[i].link;
        Sub*  s;

        if(temp == NULL) continue;

        fprintf(fp, "#:%d\n", i);
        while(temp != NULL)
        {
            fprintf(fp, "%s:%d", temp->word, temp->f_count);

            s = temp->sub_link;
            for (int j = 0; j < temp->f_count; j++)
            {
                fprintf(fp, ":%s:%d", s->f_name, s->word_count);
                s = s->link;
            }
            fprintf(fp, ":#\n");

            temp = temp->link;
        }
    }

    fclose(fp);
    return SUCCESS;
}


static void clear_database(Hash_table* ht)
{
    for (int i = 0; i < 27; i++)
    {
        Main* m = ht[i].link;

        while(m != NULL)
        {
            Main* next_m = m->link;
            Sub*  s      = m->sub_link;

            while (s != NULL)
            {
                Sub* next_s = s->link;
                free(s);
                s = next_s;
            }

            free(m);
            m = next_m;
        }
        ht[i].link = NULL;
    }
}

static void remove_file(File** head, char* f_name)
{
    File* temp = *head;
    File* prev = NULL;

    while(temp != NULL)
    {
        if(strcmp(temp->f_name, f_name) == 0)
        {
            if (prev == NULL)
                *head = temp->link;
            else
                prev->link = temp->link;

            free(temp);
            return;
        }
        prev = temp;
        temp = temp->link;
    }
}

// Update database
int update_database(char* f_name, Hash_table* ht, File** f_head)
{
    // Check the file extension
    char* ptr = strrchr(f_name, '.');
    if (ptr == NULL || (strcmp(ptr, ".txt") != 0))
        return FAILURE;

    // Update is only allowed on empty database
    for (int i = 0; i < 27; i++)
    {
        if (ht[i].link != NULL) return FAILURE;
    }

    FILE* fp = fopen(f_name, "r");
    if (fp == NULL) return FAILURE;

    char line[1024];
    int  index = -1;

    if (fscanf(fp, "%1023s", line) != 1) goto fail;

    // Check if the file is a valid backup
    if (strcmp(line, MAGIC_STRING) != 0) goto fail;

    while(fgets(line, sizeof(line), fp) != NULL)
    {

        line[strcspn(line, "\r\n")] = '\0';
        if(line[0] == '\0')  continue;

        // Get header -> #:<index>
        if(line[0] == '#' && line[1] == ':')
        {
            index = atoi(line + 2);
            if (index < 0 || index >= 27) goto fail;

            ht[index].index = index;
            continue;
        }

        // word line before any header is invalid
        if (index < 0) goto fail;

        // Word
        char* word = strtok(line, ":");
        char* count = strtok(NULL, ":");
        if (word == NULL || count == NULL) goto fail;

        int f_count = atoi(count);
        if (f_count <= 0 || strlen(word) >= 50) goto fail;

        for (int j = 0; j < f_count; j++)
        {
            char* f_name = strtok(NULL, ":");
            char* wc     = strtok(NULL, ":");
            if (f_name == NULL || wc == NULL) goto fail;

            int word_count = atoi(wc);
            if (word_count <= 0 || strlen(f_name) >= 50) goto fail;

            // Sent same file_name and word for word_count times
            for (int k = 0; k < word_count; k++)
            {
                if (insert_last_main(&ht[index].link, word, f_name) == FAILURE)
                    goto fail;
            }
        }
    }
    fclose(fp);

    //
    for (int x = 0; x < 27; x++)
    {
        for (Main* m = ht[x].link; m != NULL; m = m->link)
        {
            for (Sub* s = m->sub_link; s != NULL; s = s->link)
            {
                remove_file(f_head, s->f_name);
            }
        }
    }

    return SUCCESS;

    fail:
        fclose(fp);
        // All or nothing: no half-loaded database
        clear_database(ht);
        return FAILURE;
}
