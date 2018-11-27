Para compilar se debe usar el siguiente comando en linux:

g++ simple_interp.cpp sexprPath/libsexp.a -o readList

Para generar el archivo arm.s con las instrucciones de nombreArchivo se debe usar el comando:

./readList nombreArchivo

Si no se especifica el nombre del archivo se utilizará el archivo test1.in

Para ensamblar y ejecutar se deben seguir los siguientes pasos dentro de la raspberry:

as -o arm.o arm.s
gcc -o arm arm.o
./arm