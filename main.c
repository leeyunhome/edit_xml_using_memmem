#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define END_TAG_SIZE 23

char *memmem(char *p_src, int src_len, char *p_trg, int trg_len)
{
    char *p = NULL;
    int i;
    int j;
    p = p_src;
    for (i = 0; i < src_len - trg_len + 1; i++)
    {
        if (*p == *p_trg)
        {
            for (j = 1; j < trg_len; j++)
            {
                if (p[j] != p_trg[j])
                {
                    break;
                }
            }
            if (j == trg_len)
            {
                return p;
            }
        }
        p++;
    }
    return NULL;
}

int main()
{
    size_t content_size;
    size_t size_of_input_string;
    size_t size_of_total_buffer;

    int apply_flag = false;
    char filename[] = "org.gnome.Vino.gschema.xml";
    FILE *file;
    if ((file = fopen(filename, "r")) == NULL)
    {
        printf("Cannot open file.\n");
        exit(EXIT_FAILURE);
    }

    char string_to_input[] = "<key name='enabled' type='b'>\
<summary> Enable remote access to the desktop</summary>\
<description>\
If true, allows remote access to the desktop via the RFB \
protocol.Users on remote machines may then connect to the\
desktop using a VNC viewer.\
</description>\
<default>true</default>\
</key>";

    /* find size */
    fseek(file, 0, SEEK_END);
    content_size = ftell(file);
    size_of_input_string = strlen(string_to_input);
    size_of_total_buffer = content_size + size_of_input_string;

    printf("conent_size : %d\n", content_size);

    char *total_buffer = (char *)malloc(size_of_total_buffer);
    if (total_buffer == NULL)
    {
        printf("ERROR: malloc() failed.\n");
        exit(EXIT_FAILURE);
    }
    char *head_buffer = (char *)malloc(content_size);
    if (head_buffer == NULL)
    {
        printf("ERROR: malloc() failed.\n");
        exit(EXIT_FAILURE);
    }
    char *tail_buffer = (char *)malloc(END_TAG_SIZE);
    if (tail_buffer == NULL)
    {
        printf("ERROR: malloc() failed.\n");
        exit(EXIT_FAILURE);
    }

    rewind(file); /* set to start */

    fread(head_buffer, content_size, 1, file);

    char *buf = (char *)memmem(head_buffer, content_size, "</schema"
                                                          ">",
                               strlen("</schema"
                                      ">"));

    strncpy(tail_buffer, buf, END_TAG_SIZE);
    printf("buf : %s", buf);
    printf("tail_buffer : %s", tail_buffer);

    memcpy(total_buffer, head_buffer, content_size - 27);

    strncat(total_buffer, string_to_input, size_of_input_string);
    strncat(total_buffer, tail_buffer, END_TAG_SIZE);
    printf("total_buffer : %s", total_buffer);

    char *disable_overwrite = (char *)memmem(head_buffer, content_size, "RFB",
                                             strlen("RFB"));

    if (disable_overwrite != NULL)
    {
        apply_flag = false;
        printf("apply_flag : %d", apply_flag);
    }
    else
    {
        apply_flag = true;
        printf("apply_flag : %d", apply_flag);
    }

    FILE *file_to_write;
    if ((file_to_write = fopen("org.gnome.Vino.gschema.xml", "w")) == NULL)
    {
        printf("Cannot open file.\n");
        exit(EXIT_FAILURE);
    }

    if (apply_flag == true)
    {
        // write to file.
        fwrite(total_buffer, size_of_total_buffer, 1, file_to_write);
    }
    else
    {
        exit(EXIT_SUCCESS);
    }

    if (fclose(file) != 0)
    {
        exit(EXIT_FAILURE);
    }

    if (fclose(file_to_write) != 0)
    {
        exit(EXIT_FAILURE);
    }

    free(total_buffer);
    total_buffer = NULL;

    free(head_buffer);
    head_buffer = NULL;

    free(tail_buffer);
    tail_buffer = NULL;

    return 0;
}