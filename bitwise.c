#include <fcntl.h>
#include <stdio.h>

void display_message(char *, unsigned int );
void display_byte(unsigned int);

int main(int argc, char const *argv[]) {

  display_message("O_CREAT", O_CREAT|O_WRONLY|O_RDONLY|O_RDWR|O_APPEND|O_TRUNC);



  return 0;
}

void display_message(char *message, unsigned int value ){
  printf("%s, %d\t\t", message, value);
  display_byte(value);
  printf("\n" );
}

void display_byte(unsigned int value){
  unsigned int mask = 0xff000000;
  unsigned int shift = 256*256*256; // mover 24 bit 
  unsigned int byte, byte_iterator, bit_iterator;

  for(byte_iterator=0; byte_iterator<4; byte_iterator++ ){
    byte = (value & mask) / shift; // AND logico y luego muevo 24 bit a la derecha, queda 8 bits 
    printf(" ");
    for(bit_iterator = 0; bit_iterator < 8; bit_iterator++){
      if(byte & 0x80) // 0x80 = 1000 0000 me fijo si el bit mas significativo esta encendido
        printf("1");
      else
        printf("0");
      byte *=2; // multiplicar por 2 es lo mismo que shifl en assembler muevo 1 bit a la izquierda.
    }
    mask /= 256; // muevo 8 bits a la derecha
    shift /=256; // muevo 8 bits a la derecha
  }
}
