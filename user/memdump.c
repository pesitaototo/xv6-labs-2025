#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void memdump(char *fmt, char *data);

int
main(int argc, char *argv[])
{
  if(argc == 1){
    printf("Example 1:\n");
    int a[2] = { 61810, 2025 };
    memdump("ii", (char*) a);
    
    printf("Example 2:\n");
    memdump("S", "a string");
    
    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *) &s);

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;
    
    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");
    
    printf("Example 4:\n");
    memdump("pihcS", (char*) &example);
    
    printf("Example 5:\n");
    memdump("sccccc", (char*) &example);
  } else if(argc == 2){
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while(n < sizeof(data)){
      int nn = read(0, data + n, sizeof(data) - n);
      if(nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void
memdump(char *fmt, char *data)
{
  // data_cur tracks the current byte of data to read from
  int data_cur = 0;

  for (int i=0; fmt[i] != '\0'; i++) {
    switch(fmt[i]) {
      case('i'):
        // print next 4 bytes as 32 bit integer in decimal
        int i_result;
        // little endian
        i_result = (data[data_cur]) 
                    | (data[data_cur + 1] << 8) 
                    | (data[data_cur + 2] << 16) 
                    | (data[data_cur + 3] << 24);
        data_cur += 4;
        printf("%d\n", i_result);
        break;
      case('p'):
        // print next 8 bytes as 64 bit integer in hex
        unsigned long long hex = ((unsigned long long) data[data_cur]) 
                    | ((unsigned long long) data[data_cur + 1] << 8) 
                    | ((unsigned long long) data[data_cur + 2] << 16) 
                    | ((unsigned long long) data[data_cur + 3] << 24) 
                    | ((unsigned long long) data[data_cur + 4] << 32) 
                    | ((unsigned long long) data[data_cur + 5] << 40) 
                    | ((unsigned long long) data[data_cur + 6] << 48) 
                    | ((unsigned long long) data[data_cur + 7] << 56);
        data_cur += 8;
        printf("%llx\n", hex);
        break;
      case('h'):
        // print next 2 bytes of data as 16 bit integer in decimal
        short int result = (data[data_cur]) | (data[data_cur+1] << 8);
        
        data_cur += 2;
        printf("%d\n", result);

        break;
      case('c'):
        // print next 1 byte of data as 8 bit ASCII or 1 byte char
        char c_result = data[data_cur];
        data_cur++;
        printf("%c\n", c_result);
        break;
      case('s'):
        // next 8 bytes of data contain a 64-bit pointer to a C string; print the string
        unsigned long long s_ptr = data[data_cur]
                        | ((unsigned long long) data[data_cur+1] << 8)
                        | ((unsigned long long) data[data_cur + 2] << 16)
                        | ((unsigned long long) data[data_cur + 3] << 24)
                        | ((unsigned long long) data[data_cur + 4] << 32)
                        | ((unsigned long long) data[data_cur + 5] << 40)
                        | ((unsigned long long) data[data_cur + 6] << 48)
                        | ((unsigned long long) data[data_cur + 7] << 56);
        data_cur += 8;
        char *pstring = (char*) s_ptr;
        printf("%s\n", pstring);
        break;
      case('S'):
        // the rest of the data contains bytes of a null-terminated C string; print the string
        for (int i=data_cur; data[i] != '\0'; i++) {
          printf("%c", data[i]);
        }
        printf("\n");
        break;
      default:
        // handle case where format value is invalid
    }
  }
}
