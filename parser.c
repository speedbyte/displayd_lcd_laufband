#include stdio.h

void main (void)
{
    FILE * fd; //pointer to Textfile
    char * string [100];

    fd = fopen("/DisplayD/new.txt","r");
    if(!fd) exit(0);
    fscanf(fd,"%s",string);
    printf("%s",string);
    fclose(fd);
}
