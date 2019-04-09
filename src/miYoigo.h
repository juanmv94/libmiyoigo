#ifndef MIYOIGO_H
#define MIYOIGO_H

#include <string>
#include <vector>
#include <exception>
#include "curl/curl.h"

//Tipos de detalles de consumo
#define TIPO_DATOS "DATA"
#define TIPO_LLAMADA_FIJO "LAND_LINE_CALL"
#define TIPO_LLAMADA_MOVIL "MOBILE_CALL"
#define TIPO_SMS "SMS"
#define TIPO_MMS "MMS"

using namespace std;

//Estructuras

struct MemoryStruct {
    char *memory;
    size_t size;
};

////////////////////////////

struct Cuenta {
    string id;
    string nombre;
    string apellido1;
    string apellido2;
};

////////////////////////////

struct Consumo {
    int incluido;
    int consumido;
    int restante;
    string unidad;
    bool ilimitado;
};

struct Consumos {
    struct Consumo llamadas;
    struct Consumo datos;
    struct Consumo sms;
    struct Consumo mms;
};

////////////////////////////

struct Detalle {
    string tipo;    //Mirar defines
    string fecha;
    string hora;
    int consumido_cantidad;
    string consumido_unidad;
    float coste_cantidad;
    string coste_unidad;
    string destino;
};

//Excepciones

struct MiYoigoCredException : public std::exception {

    const char * what() const throw () {
        return "Credenciales miYoigo incorrectos";
    }
};

struct MiYoigoConException : public std::exception {

    const char * what() const throw () {
        return "Error de conexion a miYoigo";
    }
};

struct MiYoigoMemException : public std::exception {

    const char * what() const throw () {
        return "Error al asignar memoria";
    }
};

struct MiYoigoApiException : public std::exception {

    const char * what() const throw () {
        return "La respuesta de la API no tiene la estructura esperada";
    }
};

////////////////////////////

class miYoigo {
private:
    CURL *curl;
    struct curl_slist *slist;
    string authorization;
    string cid_header;
    struct MemoryStruct responseHeader;
    struct MemoryStruct responseBody;
    struct Cuenta micuenta;
    vector<string> miproductos;

    void GET_request(string url);
    void clear_all();

public:
    miYoigo(string telefono, string password);
    ~miYoigo();

    const struct Cuenta* cuenta();
    const vector<string>* productos();
    struct Consumos obtener_consumos(string telefono);
    vector<struct Detalle> obtener_detalles_consumo(string telefono);
};

#endif