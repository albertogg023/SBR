#include <stdlib.h>  // Funcion exit
#include <string.h>  // Funcion memset
#include <iostream>  // Variables cin y cout
#include <sstream>  // iostream
#include <fstream>  // lectura ficheros
#include <list> // lista enlazada
using namespace std;


//////////////////////////////////////////////////////////////
////////////        TIPOS DE DATOS            ////////////////
//////////////////////////////////////////////////////////////

typedef struct {
    string id;  // identificador de la regla
    int modoAntecedentes;   // -1=unLiteral, 0=variosLiteralesConConjunciones, 1=variosLiteralesConDisyunciones
    string * antecedentes;  // array de antecedentes de la regla en memoria dinamica
    string consecuente; // consecuente de la regla
    float factorCerteza;    // factor de certeza de la regla
} Conocimiento;


typedef struct {
    string id;  // identificador del hecho
    float factorCerteza;    // factor de certeza del hecho
} Hecho;


//////////////////////////////////////////////////////////////
////////////     FUNCIONES DEL PROGRAMA       ////////////////
//////////////////////////////////////////////////////////////

// LECTURA DE LA BASE DE CONOCIMIENTO DE UN FICHERO
int lecturaBC(string file, list<Conocimiento> & listaConocimientos){

    ifstream BC(file); // para leer del fichero
    string linea; string cadena; string basura; // para leer el contenido del fichero

    list<string> listaAntecentes;   // para guardar provisionalmente los antecedentes de la regla actual
    int numReglas; BC >> numReglas;    // leemos el numero de reglas
    BC.ignore(1, ' ');  // saltamos a la siguiente linea

    for(int i = 1; i <= numReglas; ++i){

        // Obtenemos el contenido de la linea
        getline(BC, linea); // extraemos la linea de la BC
        istringstream cadenas(linea); // obtenemos las cadenas de la linea

        // Creamos la estructura Conocimiento
        Conocimiento conocimiento;

        // Lectura idRegla:
        cadenas >> cadena;    // extraemos el id de la regla con ':' incluidos
        conocimiento.id = cadena.substr(0, cadena.size()-1);

        // Ignoramos la palabra 'si'
        cadenas >> basura;

        // Lectura de los antecedentes de la regla
        listaAntecentes.clear();    // vaciamos la lista de antecedentes
        int modoAntecedentes = -1;  // suponemos inicialmente que solo hay un literal
        while(cadenas >> cadena && cadena != "Entonces" ){
            if(cadena == "y")
                modoAntecedentes = 0;
            else if(cadena == "o")
                modoAntecedentes = 1;
            else
                listaAntecentes.push_back(cadena);
        }
        conocimiento.modoAntecedentes = modoAntecedentes;
        conocimiento.antecedentes = new string[listaAntecentes.size()+1];   // reservamos memoria para guardar los antecedentes y la marca de fin
        int indAntecedentes = 0;
        list<string>::iterator it = listaAntecentes.begin();
        while(it != listaAntecentes.end()){ // insertamos los antecedentes de la regla
            conocimiento.antecedentes[indAntecedentes++] = *it;
            ++it;
        } conocimiento.antecedentes[indAntecedentes] = "\0"; // insertamos marca de fin

        // Lectura consecuente
        cadenas >> cadena;
        conocimiento.consecuente = cadena.substr(0, cadena.size()-1);

        // Lectura FC
        cadenas >> cadena;
        conocimiento.factorCerteza = stof(cadena.substr(3));

        listaConocimientos.push_back(conocimiento); // insertamos el conocimiento en la coleccion

    }

    return 0;

}

// LECTURA DE LA BASE DE HECHOS DE UN FICHERO
int lecturaBH(string file, list<Hecho> & listaHechos){

    ifstream BH(file); // para leer del fichero
    string linea; string cadena; string basura; // para lectura del contenido del fichero

    string objetivo;
    int numHechos; BH >> numHechos;    // leemos el numero de reglas
    BH.ignore(1, ' ');  // saltamos a la siguiente linea

    for(int i = 1; i <= numHechos; ++i){

        // Obtenemos el contenido de la linea
        getline(BH, linea); // extraemos la linea de la BC
        istringstream cadenas(linea); // obtenemos las cadenas de la linea

        Hecho hecho;

        // Lectura el id del hecho
        cadenas >> cadena;
        hecho.id = cadena.substr(0,cadena.size()-1);

        // Lectura el factor de certeza del hecho
        cadenas >> cadena;
        hecho.factorCerteza = stof(cadena.substr(3));

        listaHechos.push_back(hecho);   // insertamos el hecho en la coleccion

    }

    // Ignoramos palabra objetivo
    BH >> basura;

    // Obtenemos el objetivo
    BH >> objetivo;

    return 0;

}


//////////////////////////////////////////////////////////////
////////////        PROGRAMA PRINCIPAL        ////////////////
//////////////////////////////////////////////////////////////

int main (void){

    list<Conocimiento> BC;
    lecturaBC("BC-ejemplo2.txt", BC);

    list<Hecho> BH;
    lecturaBH("BH-ejemplo2.txt", BH);

    // LIBERAR MEMORIA DINAMICA
}
