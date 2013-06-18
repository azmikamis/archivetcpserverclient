#include <stdio.h>
#include <string.h>

int main()
{
   #define CHUNK 1024 /* read 1024 bytes at a time */
   //char buf[CHUNK];
   FILE *file;
   size_t nread;

   char *buf = (char*)malloc(CHUNK);
   if (buf == NULL) {
      printf("malloc error");
   }

   file = fopen("input.txt", "rb");
   fclose(file);
   if (file) {
      /* otherwise do this.  Note 'chunk' instead of 'sizeof buf' */
      while ((nread = fread(buf, 1, CHUNK, file)) > 0) {
         fwrite(buf, 1, nread, stdout);
         if (ferror(file)) {
            printf("file error");
         }
         fclose(file);
      }
   }
   return(0);
}