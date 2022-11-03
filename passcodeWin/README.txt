IMAGEN 1, 2: mediante 2 herramientas diferentes me sale informacion distintas asi que abro el binaro para ver que en la imagen 3 el offset 0x3c donde empieza el PE header en litle endian, me dice que que en offset = x00000080 este me dice 0x014c IMAGE FILE MACHINE I386 sistema de 32 bit lo que tengo la siguiente info

Filename:  passcode.exe
Filesize:  16,766 bytes
MD5:  5D984DB6FA89BA90CF487BAE0C5DB300
SHA256:  A5A981EDC9D4933AEEE888FC2B32CA9E0E59B8945C78C9CBD84085AB8D616568
File Type: Windows PE 32-bit
Compiler: MingWin32 - Dev C++

::::::::::::::::::::::::::::::::::::Deadlisting::::::::::::::::::::::::::::::::::

Imagen 5 >> No se ve en la foto pero resta esp-88h
Luego agrega esos valores al stack en esp, luego eax apunta a la direccion donde se guardo la variable en memoria y hago un strlen, lo
guarda en eax y lo comparo con 11h que es 17 en hexadecimal, si la resta de cmp, si el valor de la comparacion no es zero osea la resta da distinto de 0, me dice incorrect password...

Osea que la contrase;a tiene que ser de 17 caracteres.


(ABRIR PANTALLA COMPLETA README.txt)

 	0 	1 	2 	3 	4 	5 	6 	7 	8 	9 	A 	B 	C 	D 	E 	F
-60h 									03 	00 	00 	00 	05 	00 	00 	00
-50h 	07 	00 	00 	00 	0e 	00 	00 	00 	10 	00 	00 	00 				
-40h 																
-30h 								
-20h 							
-10h 																
ebp


Imagen6 >> Hace un loop de 5 saltos edx apunta a la memoria ebp-8 y primero vemos la intruccion [ebp+eax*4-5ch], tranladamos esto a eax = 5 y va decrementando en cada loop lo que quedaria 


[ebp+5*4-5ch] -> [ebp-48h] = 10h
[ebp+4*4-5ch] -> [ebp-4Ch] = 0eh
[ebp+3*4-5ch] -> [ebp-50h] = 7
[ebp+2*4-5ch] -> [ebp-54h] = 5
[ebp+1*4-5ch] -> [ebp-58h] = 3

luego todo junto -> add     edx, [ebp+eax*4-5Ch] = 

eax = 5;  edx = ebp-8+10h;  edx = ebp+8
eax = 4;  edx = ebp-8+0eh;  edx = ebp+6
eax = 3;  edx = ebp-8+7;    edx = ebp-1
eax = 2;  edx = ebp-8+5;    edx = ebp-3
eax = 1;  edx = ebp-8+3;    edx = ebp-5

Por ultimo lo guardamos en eax y le restamos -20h

from eax = 5;  eax = ebp+8-20h;  eax = ebp-18h
from eax = 4;  eax = ebp+6-20h;  eax = ebp-1ah
from eax = 3;  eax = ebp-1-20h;  eax = ebp-21h
from eax = 5;  eax = ebp-3-20h;  eax = ebp-23h
from eax = 5;  eax = ebp-5-20h;  eax = ebp-25h

y comparamos esa direccion con 65h, lo que es igual a 'e' en ASCII, osea tenemos que tener 5 e en la contrase;a en posiciones especificas... 


	0 	1 	2 	3 	4 	5 	6 	7 	8 	9 	A 	B 	C 	D 	E 	F
-60h 									03 	00 	00 	00 	05 	00 	00 	00
-50h 	07 	00 	00 	00 	0e 	00 	00 	00 	10 	00 	00 	00 				
-40h 																
-30h 					 		 		 	 	 	e 	 	e 	 	e
-20h 	 	 	 	 	 	 	e 	 	e 							
-10h 																
ebp 	

Imagen 7 >> comparo 1Ah y 25h con AND y lo almaceno en 2Ch luego se compara con 0FFFFFFh para almenos 3 caracteres, llamos a strcmp 
le paso ere y lo que hay en 2Ch luego hago un test 

0 	1 	2 	3 	4 	5 	6 	7 	8 	9 	A 	B 	C 	D 	E 	F
-60h 									03 	00 	00 	00 	05 	00 	00 	00
-50h 	07 	00 	00 	00 	0e 	00 	00 	00 	10 	00 	00 	00 				
-40h 																
-30h 					e 	r 	e 		X 	X 	X 	e 	r 	e 	X 	e
-20h 	X 	X 	X 	X 	X 	X 	e 	r 	e 							
-10h 																
ebp 	


En la imagen 8 y nueve se siguen agarrando 3 caracteres y reemplazando con posiciones, por ejemplo en la 8 se suman dos registros y si es 0 entonces sigo sin ir al incorrect password lo que me dice que ebp-22 y ebp-1E tiene que sumar 0x40h (0x20 en ASCII) luego de seguir las comparaciones obtengo mi contrase;a...



	0 	1 	2 	3 	4 	5 	6 	7 	8 	9 	A 	B 	C 	D 	E 	F
-60h 									03 	00 	00 	00 	05 	00 	00 	00
-50h 	07 	00 	00 	00 	0e 	00 	00 	00 	10 	00 	00 	00 				
-40h 																
-30h 					e 	r 	e 		A 	u 	d 	e 	r 	e 		e
-20h 	s 	t 		F 	a 	c 	e 	r 	e 							
-10h 																
ebp 	


"Audere est Facere"					

