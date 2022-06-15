#include <stdio.h>
#include <string.h>
#include "checksum.h"

void substring(char s[], char sub[], int p, int l)
{
   int c = 0;
   while (c < l) {
      sub[c] = s[p+c-1];
      c++;
   }
   sub[c] = '\0';
}

static char f_crc32[16];

extern char *file_crc32(char device[], char path[], char fn[])
{
  char tmp[32];
  uint32_t t_crc32 = 0xffffffffL;
  char full_path[256];
  //int chunks_curr = 1;
  int bytes_read;
  //Build full_path string
	snprintf(full_path, sizeof(full_path), "%s%s%s", device, path, fn);
  FILE *fp = fopen(full_path, "rb");
  if (!fp)
  {
        printf("ERROR: Unable to open %s for reading \n", full_path);
	return("ERROR: Unable to open file for reading \n");
  }
  //read file, store length in len
  fseek(fp,0,SEEK_END);
  long len = ftell(fp);
  long fsize = len;
  fseek(fp,0,SEEK_SET);
  printf("Filesize: %lu bytes \n", fsize);
  //4MB File Buffer. If less than that read entire into buf
  if (len <= 4194304) {
	char buf[len];
	while((fread(buf, 1, len, fp)) > 0){
		printf("%lu bytes read \n", len);
	}
	//Close the file
	fclose(fp);

	//Use sprintf to store crc_32() return value in tmp
	//If file is larger than buffer, update_crc_32() will
	//need to be looped to get large file CRC32
	sprintf(tmp,"%lX", crc_32(buf, len));
  //4MB File Buffer. If more than that read byte by byte into ch
  //Calling update_crc_32 and passing the old CRC32 and new byte each time.
  } else {
	char buf[1];
	int ch;
	ch=fgetc(fp);
	t_crc32 = update_crc_32(t_crc32,(unsigned char) ch);
	sprintf(tmp,"%lX", t_crc32);
	bytes_read = sizeof(buf);
	while((ch=fgetc(fp)) != EOF){
		t_crc32 = update_crc_32(t_crc32, (unsigned char) ch);
		bytes_read++;
		//chunks_curr++;
	}
	//Close the file.
	fclose(fp);

	//crc lib requires this operation be preformed on final value
	t_crc32 ^= 0xffffffffL;
	//Copy the final CRC32 to tmp
	sprintf(tmp,"%lX",t_crc32);
  }

  //scr_printf("Debug: %s\n", tmp);
  //We only need the last 8 bytes of crc_32 return value
  //Sometimes it is twice as long preceded by 0xffffffff
  //copy processed value to f_crc32
  //scr_printf("Debug (tmp): %lx \n", tmp);
  if (strlen(tmp)>=9){
    substring(tmp,f_crc32,9,8);
  } else if (strlen(tmp)<=7) {
	    if (strlen(tmp) == 7) {
		sprintf(f_crc32,"0%s",tmp);
	    } else if (strlen(tmp) == 6) {
		sprintf(f_crc32,"00%s",tmp);
	    } else if (strlen(tmp) == 5) {
		sprintf(f_crc32,"000%s",tmp);
	    } else if (strlen(tmp) == 4) {
		sprintf(f_crc32,"0000%s",tmp);
	    } else if (strlen(tmp) == 3) {
		sprintf(f_crc32,"00000%s",tmp);
	    } else if (strlen(tmp) == 2) {
		sprintf(f_crc32,"000000%s",tmp);
	    } else if (strlen(tmp) == 1) {
		sprintf(f_crc32,"0000000%s",tmp);
	    }
    //substring(tmp,f_crc32,0,8);
  } else {
    substring(tmp,f_crc32,1,8);
  }
  //Display CRC32
  //scr_printf("CRC32: %s \n",f_crc32);
  return f_crc32;
}
