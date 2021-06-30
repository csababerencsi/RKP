#include "myHeader.h"

int main(int argc, char *argv[])
{
    int f = 0;
    if (argc > 2)
    {
        fprintf(stderr, "Hiba! Túl sok argumentumot adtál meg!\n");
        return 1;
    }
    else if (argc == 2)
    {
        if (strcmp(argv[1], "--version") == 0)
        {
            printf("Version number: 6.0\n");
            printf("Creation date: 2021.02.17\n");
            printf("Last modified: 2021.04.25\n");
            printf("Developer: Berencsi Csaba\n");
            return 0;
        }
        if (strcmp(argv[1], "--help") == 0)
        {
            printf("Functions:\n");
            printf("|--help| --> kiírja a parancssori argumentumokat\n");
            printf("|--version| --> kiírja a program verziószámát, elkészítésének dátumát, és a program készítőjét\n");
            printf("|'filename.bmp'| --> a megadott bmp fájlból dekódolja a kódolt szöveget\n");
            printf("' ' --> argumentum nélkül megnyit egy előre beállított állományt\n");
            return 0;
        }
        if (strstr(argv[1], ".bmp") == 0)
        {
            printf("Hiba! Adj meg egy .bmp kiterjesztésű fájlt!\n");
            return 1;
        }

        f = open(argv[1], O_RDONLY);
    }
    else
    {
        f = BrowseForOpen();
    }
    if (f == 0)
    {
        signal(SIGALRM, SIG_DFL);
        signal(SIGINT, SIG_DFL);
    }
    else
    {
        signal(SIGALRM, WhatToDo);
        signal(SIGINT, WhatToDo);
    }
    alarm(1);
    int NumCh = 0;
    char *Pbuff = ReadPixels(f, &NumCh);
    char *message = Unwrap(Pbuff, NumCh);
    alarm(0);
    int send = 1;
    char *neptunID = "YKS33Y";
    send = Post(neptunID, message, NumCh);
    if (send != 0)
    {
        fprintf(stderr, "Hiba! %d\n", send);
        return 1;
    }
    else
    {
        printf("Az adat sikeresen elküldve a szervernek.\n");
        printf("Dekódolt szöveg: %s\n", message);
    }

    return 0;
}