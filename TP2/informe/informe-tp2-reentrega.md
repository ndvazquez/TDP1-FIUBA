# Informe TP2 - Reentrega

En este informe se detallará brevemente los cambios realizados a la entrega anterior. Adicionalmente se adjunta el git diff entre el commit de la primer entrega y el de esta reentrega, para que sea más sencillo observar los cambios realizados.

# Changelog

* Usar `std::vector` en `tp.cpp`: Se realizó el cambio, y en consecuencia se modificó en la clase Brainfuck también.

* Interpreter -> Brainfuck: Se cambió el nombre de esta clase. Ahora el script se guarda en un `std::vector<char>` en lugar de `std::string`.

* Brainfuck: Se pidió imprimir un error por `std::cerr` en caso de toparse con un caracter inválido, pero no se pasan las pruebas del Sercom ya que tanto la salida estandar como la salida de error estandar se redirigen al archivo de salida, por lo cual en caso de toparse con caracteres inválidos se agrega el mensaje de error al archivo de salida y el diff falla. Entiendo la idea de este cambio pedido pero di marcha atrás en la última entrega por esta razón!

* Interpreter (nuevo): Esta clase se creo para encapsular el modo de ejecución `interprete`.

* Threadpool (nuevo): Esta clase se creo para encapsular el modo de ejecución `thread-pool`.

* Se eliminó la busy wait en el modo `thread-pool`.

* PriorityQueueProtected: Se eliminaron los métodos públicos para saber si se encuentra vacía o "finalizada", y la race condition ya no está. Ahora la condición para cortar el trabajo de los workers es encolar containers de metadata inválidos.

* Worker: Se modificó acorde al cambio realizado en PriorityQueueProtected.

* Tokenizer: Ya no se borran subcadenas de la cadena a tokenizar, se realiza el trabajo utilizando iteradores y obteniendo los tokens a medida que se avanza, sin modificar la línea a procesar.

* ScriptContainer: El atributo `_priority` ahora es `int`.
