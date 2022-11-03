Imagen 1: vemos que compara el valor de la memoria var_414 con 7, si no es igual me dice Incorrect Password, pero si es igual me va a otra direccion, con esto podemos deducir que el tama;o de la contrase;a tiene que ser 7 caracteres.

Image2: Podemos deducir que copia la contrase;a en otra variable y la compara con 0x2de en decimal 734:

Con esta info podemos crar una contrase;a 734/7. 104*7+6=734

Algunas contrase;as que pasan:

hhhhhhn 104*6+110
iiiiiih 105*6+104
nnnnnnJ 110*6+74

//Con este comando determinamos que es de bintype elf osea que puede ser ejecutado en linux.

rabin2 -I passcode

arch     x86
baddr    0x0
binsz    14902
bintype  elf
bits     64
canary   true
class    ELF64
compiler GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0
crypto   false
endian   little
havecode true
intrp    /lib64/ld-linux-x86-64.so.2
laddr    0x0
lang     c
linenum  true
lsyms    true
machine  AMD x86-64 architecture
nx       true
os       linux
pic      true
relocs   true
relro    full
rpath    NONE
sanitize false
static   false
stripped false
subsys   linux
va       true

