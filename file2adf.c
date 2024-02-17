/*
   file2adf.c
   Copyright 2019 Dmitry Geurkov (troydm)

   Permission is hereby granted, free of charge, to any person obtaining a copy of this software
   and associated documentation files (the "Software"), to deal in the Software without restriction,
   including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
   subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all copies 
   or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
   INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
   OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <adflib.h>

#define BUF_SIZE 4096

int main(int argc, char *argv[]){
    struct AdfDevice *flop;
    struct AdfVolume *vol;
    struct AdfFile *file;
    int n;
    unsigned char buf[BUF_SIZE];
    FILE* in;
    char *adfile, *name;
    RETCODE rc;

    if(argc < 4) {
        fprintf(stderr, "file2adf [file [file2]] adfile name\n");
        return EXIT_FAILURE;
    }

    adfile = argv[argc-2];
    name = argv[argc-1];
    printf("creating %s floppy dump with name %s\n", adfile, name);

    adfEnvInitDefault();

    flop = adfCreateDumpDevice(adfile, 80, 2, 11);
    if(!flop) {
        fprintf(stderr, "couldn't create floppy dump device\n");
        return EXIT_FAILURE;
    }

    rc = adfCreateFlop(flop, name, FSMASK_DIRCACHE);
    if(rc != RC_OK) {
        fprintf(stderr, "couldn't create floppy filesystem\n");
        return EXIT_FAILURE;
    }

    vol = adfMount(flop, 0, FALSE);
    if(!vol) {
        fprintf(stderr, "couldn't mount floppy\n");
        return EXIT_FAILURE;
    }

    for(int i = 1; i < argc-2; i++){
        char *filename;
        in = fopen(argv[i],"rb");
        if (!in) {
            fprintf(stderr, "couldn't open %s file\n", argv[i]);
            return EXIT_FAILURE;
        };
        filename = strdup(argv[i]);
        printf("adding %s to floppy\n", filename);
        file = adfFileOpen(vol, basename(filename), ADF_FILE_MODE_WRITE);

        n = fread(buf,sizeof(unsigned char),BUF_SIZE,in);
        while(!feof(in)) {
            adfFileWrite(file, n, buf);
            n = fread(buf,sizeof(unsigned char),BUF_SIZE,in);
        }
        if (n > 0)
            adfFileWrite(file, n, buf);

        free(filename);
        fclose(in);
        adfFileClose(file);
    }

    adfUnMount(vol);
    adfUnMountDev(flop);

    adfEnvCleanUp();
    return EXIT_SUCCESS;
}
