# libmiyoigo

Una librería C++ para conectar con la API de miYoigo y obtener consumos y detalles de llamadas/SMS/datos del mes entre otros.
> A C++ library for conecting miYoigo API and get consumptions and calls/SMS/data details among others.

## Compatibilidad y dependencias

Esta librería es compilable tanto en linux como en Windows (probado en MinGW) y requiere las siguientes dependencias:

* [cpp-base64](https://github.com/ReneNyffenegger/cpp-base64)
* [JJSON](https://github.com/juanmv94/JJSON)
* [libCURL](https://curl.haxx.se/libcurl/)

Las dos primeras son automáticamente descargadas al ejecutar los scripts de compilación.

La librería libCURL se incluye para Windows mediante los *\*.dll* incluidos. En Linux Ubuntu/Debian/Raspbian podéis descargar esta librería mediante el comando:
> sudo apt-get install libcurl-dev

## La librería

### Estructuras

##### MemoryStruct

Para uso interno de la librería. Consta de un puntero a memoria y el tamaño de memoria asignada.

##### Cuenta

En esta estructura se almacenan el nombre y apellidos del propietario de la cuenta, así como su id único.

##### Consumo

Se refiere a uno de los consumos mensuales, ya sea de llamadas, SMS, o datos. Contiene la cantidad consumida, restante, contratada, la unidad (ej: MB para los datos, o segundos para las llamadas), y un booleano que indica si es ilimitado (Cantidad contratada = ∞).

##### Consumos

Contiene una estructura *Consumo* por cada uno de los distintos tipos: llamada, SMS, MMS, datos.

##### Detalle

Dentro de la lista detallada del consumo, cada detalle contiene el tipo de actividad realizada (llamada, SMS, MMS, datos. Mirar #defines), la fecha y hora, cantidad consumida (y su correspondiente unidad), coste, y destino (solo llamadas, SMS, y MMS).

### Excepciones

##### MiYoigoCredException

Se lanza cuando se crea un objeto miYoigo proporcionando credenciales incorrectos

##### MiYoigoConException

Se lanza cuando se produce un error de conexión con miYoigo (ej: internet desconectado, servidor caído)

##### MiYoigoMemException

Se lanza en el caso muy improbable de que la aplicación se quede sin memoria descargando las respuestas del servidor.

##### MiYoigoApiException

Se lanza en el caso de que la respuesta proporcionada por el servidor tras una petición no sea como se espera. Puede deberse a un cambio futuro en la API de miYoigo, o a que el producto del que se obtienen los datos no es un teléfono móvil, por ejemplo.

### Constructores y métodos

##### miYoigo(string telefono, string password)

Crea un nuevo objeto *miYoigo* haciendo login con los credenciales proporcionados. Si los credenciales son incorrectos o la conexión a miYoigo no se realiza correctamente, lanza una excepción que podremos capturar.

##### const struct Cuenta\* cuenta()

Obtiene los datos obtenidos de la cuenta con la que se ha hecho login en el constructor.

##### const vector<string>\* productos()

Obtiene la lista de productos contratados (números de telefono) obtenidos por la cuenta con la que se ha hecho login en el constructor.

##### struct Consumos obtener\_consumos(string telefono)

Realiza una petición a la API de miYoigo para obtener los consumos mensuales (contratado, consumido, restante) para un número de telefono movil (mirar estructura *Consumos*).

##### vector<struct Detalle> obtener\_detalles\_consumo(string telefono)

Realiza una petición a la API de miYoigo para obtener la lista detallada de actividades realizadas (llamadas, SMS, conexiones de datos) ordenadas por fecha para un teléfono móvil (mirar estructura *Detalle*).

## Ejemplo

Se incluye un ejemplo del uso de la librería, asi como dos scripts para compilarlo tanto en Linux como en Windows.

## Acerca de

Si teneis alguna duda o quereis ver más proyectos interesantes, podeis visitar mi blog <https://technology-hellin.blogspot.com>
