#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include "miYoigo.h"

int main(int argc, char **argv) {
    if (argc != 4) {
        cout << "Debes pasar telefono, password, y producto como parametros\n";
        return 0;
    }
    try {
        miYoigo my(argv[1], argv[2]);
		bool encontrado=false;
        for (vector<string>::const_iterator it = my.productos()->begin(); it != my.productos()->end(); it++) {
            if (strcmp(it->data(),argv[3])==0) { //verificamos que el producto existe
				encontrado=true;
				break;
			}
        }
		if (!encontrado) {
			cout << "El producto no esta contratado\n";
			return 0;
		}
		float datos[31] = {};
		int maxfecha=0;
		vector<struct Detalle> detalles = my.obtener_detalles_consumo(argv[3]);
		for (vector<struct Detalle>::iterator itd = detalles.begin(); itd != detalles.end(); itd++) {
            if (itd->tipo == TIPO_DATOS) {
				int dia=atoi(itd->fecha.substr(8,2).data());
				if (dia<1 || dia>31) {
					cout << "La fecha de consumo " << itd->fecha << " es incorrecta\n";
					return 0;
				}
				datos[dia-1]+=itd->consumido_cantidad;
				if (dia>maxfecha) maxfecha=dia;
			}
		}
		if (maxfecha==0) {
			cout << "No se han consumido datos\n";
			return 0;
		}
		std::ofstream outfile("yoigo.dat");
		for (int i=0;i<maxfecha;i++) {
			outfile << (i+1) << " " << datos[i] << std::endl;
		}
		outfile.close();
		
    } catch (exception& e) {
        cout << "Error: " << e.what() << "\n";
    }
    return 0;
}