#include "miYoigo.h"
//#include <iostream>
#include <cstring>
#include "cpp-base64/base64.h"
#include "JJSON/c++/JJSON.h"

const char * host_header = "Host: api-miyoigo.yoigo.com";

//Callback libCurl en memoria

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *) userp;
    char* mempointer = (char*) realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
        /* out of memory! */
        throw MiYoigoMemException();
    }
    mem->memory = mempointer;
    memcpy(mempointer + mem->size, contents, realsize);
    mem->size += realsize;
    mempointer[mem->size] = 0;

    return realsize;
}

miYoigo::miYoigo(string telefono, string password) {
    CURLcode res;
    long response_code;

    responseHeader.memory = (char*) malloc(1);
    responseHeader.memory[0] = 0;
    responseHeader.size = 0;

    responseBody.memory = (char*) malloc(1);
    responseBody.memory[0] = 0;
    responseBody.size = 0;

    curl = curl_easy_init();
    //curl_easy_setopt(curl, CURLOPT_PROXY, "http://127.0.0.1:8888");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
	curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &responseBody);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void *) &responseHeader);

    slist = curl_slist_append(NULL, host_header);
    slist = curl_slist_append(slist, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);

    curl_easy_setopt(curl, CURLOPT_URL, "https://api-miyoigo.yoigo.com/users-api/signing/login");
    string postreq = "{\"userId\":\"" + telefono + "\",\"password\":\"" + base64_encode((unsigned char*) password.c_str(), password.length()) + "\",\"brand\":\"yoigo\"}";
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postreq.c_str());

    try {
        res = curl_easy_perform(curl);
    } catch (MiYoigoMemException& e) {
        clear_all();
        throw e;
    }

    if (res != CURLE_OK) throw MiYoigoConException();
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    if (response_code == 400) {
        clear_all();
        throw MiYoigoCredException();
    }
    if (response_code != 200) {
        clear_all();
        throw MiYoigoApiException();
    }

    try {
        string header = responseHeader.memory;
		//cout << header;
        string subheader = header.substr(header.find("Authorization:"), string::npos);
        authorization = subheader.substr(0, subheader.find("\r\n"));
    } catch (exception& e) { //No encontramos valor de "authorization"
        clear_all();
        throw MiYoigoApiException();
    }

    //Obtenemos datos de la cuenta
    Elemento obtenido = JJSON::parse(responseBody.memory);
    try {
        if (obtenido.get_tipo() != JJSON_Root) throw MiYoigoApiException();
        Raiz* r = obtenido.get_root();
        Nodo* id = r->find("id");
        Nodo* nombre = r->find("firstName");
        Nodo* apellido1 = r->find("midName");
        Nodo* apellido2 = r->find("lastName");
        Nodo* productos = r->find("products");
        if (id == nullptr || nombre == nullptr || apellido1 == nullptr || apellido2 == nullptr || productos == nullptr ||
                id->elemento.get_tipo() != JJSON_String || nombre->elemento.get_tipo() != JJSON_String ||
                apellido1->elemento.get_tipo() != JJSON_String || apellido2->elemento.get_tipo() != JJSON_String ||
                productos->elemento.get_tipo() != JJSON_Vector) throw MiYoigoApiException();
        micuenta.id = *id->elemento.get_string();
        micuenta.nombre = *nombre->elemento.get_string();
        micuenta.apellido1 = *apellido1->elemento.get_string();
        micuenta.apellido2 = *apellido2->elemento.get_string();
        vector<Elemento>* prod_e = productos->elemento.get_vector();
        for (vector<Elemento>::iterator it = prod_e->begin(); it != prod_e->end(); ++it) {
            if (it->get_tipo() != JJSON_String) throw MiYoigoApiException();
            miproductos.push_back(*it->get_string());
        }
    } catch (MiYoigoApiException& e) {
        obtenido.clear();
        clear_all();
        throw e;
    }
    obtenido.clear();

    //Preparamos librería curl para resto de peticiones GET
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    curl_slist_free_all(slist);
    cid_header = "customer-id: " + micuenta.id;
    slist = curl_slist_append(NULL, host_header);
    slist = curl_slist_append(slist, authorization.c_str());
    slist = curl_slist_append(slist, cid_header.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
}

miYoigo::~miYoigo() {
    clear_all();
}

void miYoigo::clear_all() {
    free(responseHeader.memory);
    free(responseBody.memory);
    curl_slist_free_all(slist);
    curl_easy_cleanup(curl);
}

void miYoigo::GET_request(string url) {
    CURLcode res;
    long response_code;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    responseHeader.size = 0;
    responseBody.size = 0;

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) throw MiYoigoConException();
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    //cout << response_code << "\n" << responseBody.memory << "\n";
    if (response_code == 400) throw MiYoigoCredException();
    if (response_code != 200) throw MiYoigoApiException();
}

const struct Cuenta* miYoigo::cuenta() {
    return (const struct Cuenta *) &micuenta;
}

const vector<string>* miYoigo::productos() {
    return (const vector<string>*)&miproductos;
}

struct Consumos miYoigo::obtener_consumos(string telefono) {
    GET_request("https://api-miyoigo.yoigo.com/consumption-api/customers/" + micuenta.id + "/products/" + telefono + "/lines/" + telefono + "/consumption/mobile");

    //Parseamos JSON obtenido
    Consumos c;
    Nodo * consumos[4];
    struct Consumo * c_st[] = {&c.llamadas, &c.datos, &c.mms, &c.sms};

    JJSON::JJSON_Integers = true;
    Elemento obtenido = JJSON::parse(responseBody.memory);
    try {
        if (obtenido.get_tipo() != JJSON_Root) throw MiYoigoApiException();
        Raiz* r = obtenido.get_root();
        consumos[0] = r->find("calls");
        consumos[1] = r->find("data");
        consumos[2] = r->find("mms");
        consumos[3] = r->find("sms");

        //data lleva dentro otra raiz de la que obtenemos "amount"
        if (consumos[1] == nullptr || consumos[1]->elemento.get_tipo() != JJSON_Root) throw MiYoigoApiException();
        consumos[1] = consumos[1]->elemento.get_root()->find("amount");

        //Checkeamos nodos obtenidos, y rellenamos estructura con punteros en c_st
        for (int i = 0; i < 4; i++) {
            if (consumos[i] == nullptr || consumos[i]->elemento.get_tipo() != JJSON_Root) throw MiYoigoApiException();
            Raiz* rn = consumos[i]->elemento.get_root();

            Nodo* consumido = rn->find("consumed");
            Nodo* unidad = rn->find("unit");
            Nodo* ilimitado = rn->find("unlimited");

            if (consumido == nullptr || unidad == nullptr || ilimitado == nullptr ||
                    consumido->elemento.get_tipo() != JJSON_Integer || unidad->elemento.get_tipo() != JJSON_String ||
                    ilimitado->elemento.get_tipo() != JJSON_Boolean) throw MiYoigoApiException();

            c_st[i]->consumido = consumido->elemento.get_integer();
            c_st[i]->unidad = *unidad->elemento.get_string();
            c_st[i]->ilimitado = ilimitado->elemento.get_boolean();

            if (!c_st[i]->ilimitado) //Si no es ilimitado
            {
                Nodo* incluido = rn->find("included");
                Nodo* restante = rn->find("remaining");

                if (incluido == nullptr || restante == nullptr ||
                        incluido->elemento.get_tipo() != JJSON_Integer ||
                        restante->elemento.get_tipo() != JJSON_Integer) throw MiYoigoApiException();

                c_st[i]->incluido = incluido->elemento.get_integer();
                c_st[i]->restante = restante->elemento.get_integer();
            }
        }
    } catch (MiYoigoApiException& e) {
        obtenido.clear();
        throw e;
    }
    obtenido.clear();
    return c;
}

vector<struct Detalle> miYoigo::obtener_detalles_consumo(string telefono) {
    GET_request("https://api-miyoigo.yoigo.com/consumption-api/customers/" + micuenta.id + "/products/" + telefono + "/lines/" + telefono + "/consumption/mobile/detailed");

    //Parseamos JSON obtenido
    JJSON::JJSON_Integers = false;
    Elemento obtenido = JJSON::parse(responseBody.memory);
    vector<struct Detalle> v;
    try {
        if (obtenido.get_tipo() != JJSON_Root) throw MiYoigoApiException();
        Raiz* r = obtenido.get_root();
        Nodo* n = r->find("consumptions");
        if (n == nullptr || n->elemento.get_tipo() != JJSON_Vector) throw MiYoigoApiException();
        vector<Elemento>* ve = n->elemento.get_vector();

        //Procesamos cada uno de los datalles de consumos
        for (vector<Elemento>::iterator it = ve->begin(); it != ve->end(); ++it) {
            if (it->get_tipo() != JJSON_Root) throw MiYoigoApiException();
            Raiz* re = it->get_root();
            Nodo* nvd = re->find("details"); //detalles para cada día
            if (nvd == nullptr || nvd->elemento.get_tipo() != JJSON_Vector) throw MiYoigoApiException();
            vector<Elemento>* vd = nvd->elemento.get_vector();
            for (vector<Elemento>::iterator itd = vd->begin(); itd != vd->end(); ++itd) {
                if (itd->get_tipo() != JJSON_Root) throw MiYoigoApiException();
                Raiz* rd = itd->get_root();

                Nodo* nconsumed = rd->find("consumed");
                Nodo* ncost = rd->find("cost");
                Nodo* ndate = rd->find("dateTime");
                Nodo* ntype = rd->find("type");
                Nodo* nunit = rd->find("unit");

                if (nconsumed == nullptr || ncost == nullptr || ndate == nullptr || ntype == nullptr || nunit == nullptr ||
                        nconsumed->elemento.get_tipo() != JJSON_Float || ncost->elemento.get_tipo() != JJSON_Root ||
                        ndate->elemento.get_tipo() != JJSON_String || ntype->elemento.get_tipo() != JJSON_String ||
                        nunit->elemento.get_tipo() != JJSON_String) throw MiYoigoApiException();

                Raiz* rc = ncost->elemento.get_root();

                Nodo* namount = rc->find("amount");
                Nodo* ncurrency = rc->find("currencyCode");

                if (namount == nullptr || ncurrency == nullptr || namount->elemento.get_tipo() != JJSON_Float ||
                        ncurrency->elemento.get_tipo() != JJSON_String) throw MiYoigoApiException();

                //Una vez obtenidos todos los nodos y checkeados, rellenamos el detalle
                struct Detalle d;
                d.tipo = *ntype->elemento.get_string();
                d.consumido_cantidad = int(nconsumed->elemento.get_float());
                d.consumido_unidad = *nunit->elemento.get_string();
                d.coste_cantidad = namount->elemento.get_float();
                d.coste_unidad = *ncurrency->elemento.get_string();

                //fecha y hora
                d.fecha = ndate->elemento.get_string()->substr(0, 10);
                d.hora = ndate->elemento.get_string()->substr(11, 8);

                if (d.tipo != TIPO_DATOS) {
                    Nodo* ndestination = rd->find("destination");
                    if (ndestination == nullptr ||
                            ndestination->elemento.get_tipo() != JJSON_String) throw MiYoigoApiException();
                    d.destino = *ndestination->elemento.get_string();
                }

                v.push_back(d);
            }
        }

    } catch (MiYoigoApiException& e) {
        obtenido.clear();
        throw e;
    }
    obtenido.clear();
    return v;
}
