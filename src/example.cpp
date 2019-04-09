#include <iostream>
#include <string>
#include <exception>
#include "miYoigo.h"

using namespace std;

void print_consumo(Consumo* c) {
    if (c->ilimitado) {
        cout << "Consumido " << c->consumido << "(" << c->unidad << ")\n";
    } else {
        cout << "Consumido " << c->consumido << " de " << c->incluido << "(" << c->unidad << ")\n";
        cout << "Restante: " << c->restante << "\n";
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        cout << "Debes pasar telefono y password como parametros\n";
        return 0;
    }
    try {
        miYoigo my(argv[1], argv[2]);
        cout << "DATOS DE CUENTA OBTENIDOS\n";
        cout << "-------------------------\n";
        cout << "NOMBRE: " << my.cuenta()->nombre << " " << my.cuenta()->apellido1 << " " << my.cuenta()->apellido2 << "\n";
        cout << "PRODUCTOS:\n";
        for (vector<string>::const_iterator it = my.productos()->begin(); it != my.productos()->end(); it++) {
            cout << "=========\n" << *it << "\n=========\n";
            if (it->at(0) == '6' || it->at(0) == '7') { //verificamos que el producto es una linea movil
                Consumos c = my.obtener_consumos(*it);
                cout << "\nLLAMADAS:\n";
                print_consumo(&c.llamadas);
                cout << "\nDATOS:\n";
                print_consumo(&c.datos);
                cout << "\nMMS:\n";
                print_consumo(&c.mms);
                cout << "\nSMS:\n";
                print_consumo(&c.sms);
                cout << "\n\n\n";

                vector<struct Detalle> detalles = my.obtener_detalles_consumo(*it);
                for (vector<struct Detalle>::iterator itd = detalles.begin(); itd != detalles.end(); itd++) {
                    if (itd->tipo == TIPO_DATOS) cout << "Consumo de datos: ";
                    else cout << "Consumo tipo " << itd->tipo << " con destino " << itd->destino << ": ";
                    cout << itd->consumido_cantidad << " " << itd->consumido_unidad <<
                            " a las " << itd->hora << " del " << itd->fecha <<
                            " (" << itd->coste_cantidad << ' ' << itd->coste_unidad << ")\n";
                }
            } else {
                cout << "\nNo es linea movil:\n";
            }
        }
    } catch (exception& e) {
        cout << "Error: " << e.what() << "\n";
    }
    return 0;
}