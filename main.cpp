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
    string id;
    int modoAntecedentes;
    string * antecedentes;
    string consecuente;
    float factorCerteza;
} Conocimiento;


typedef struct {
    string id;
    float factorCerteza;
} Hecho;


//////////////////////////////////////////////////////////////
////////////     FUNCIONES DEL PROGRAMA       ////////////////
//////////////////////////////////////////////////////////////

int lecturaBC(){

    ifstream BC("BC-ejemplo2.txt"); // para leer del fichero
    string linea; string cadena; string basura; // para leer el contenido del fichero

    list<string> listaAntecentes;   // para guardar provisionalmente los antecedentes de la regla actual
    int numReglas; BC >> numReglas;    // leemos el numero de reglas
    BC.ignore(1, ' ');  // saltamos a la siguiente linea

    for(int i = 1; i <= numReglas; ++i){

        // Obtenemos el contenido de la linea
        getline(BC, linea); // extraemos la linea de la BC
        istringstream cadenas(linea); // obtenemos las cadenas de la linea

        // Creamos la estructura Conocimiento
        Conocimiento con;

        // Lectura idRegla:
        cadenas >> cadena;    // extraemos el id de la regla con ':' incluidos
        con.id = cadena.substr(0, cadena.size()-1);
        cout << con.id << " ";

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
            else{
                listaAntecentes.push_back(cadena);
                cout << cadena << " ";
            }
        }
        con.modoAntecedentes = modoAntecedentes;
        cout << con.modoAntecedentes << " ";
        con.antecedentes = new string[listaAntecentes.size()+1];   // reservamos memoria para guardar los antecedentes y la marca de fin
        int indAntecedentes = 0;
        list<string>::iterator it = listaAntecentes.begin();
        while(it != listaAntecentes.end()){ // insertamos los antecedentes de la regla
            con.antecedentes[indAntecedentes++] = *it;
            ++it;
        } con.antecedentes[indAntecedentes] = "\0"; // marca de fin

        // Lectura consecuente,
        cadenas >> cadena;
        con.consecuente = cadena.substr(0, cadena.size()-1);
        cout << con.consecuente << " ";

        // Lectura FC
        cadenas >> cadena;
        con.factorCerteza = stof(cadena.substr(3));
        cout << con.factorCerteza;
        cout << endl;
    }

}


int lecturaBH(){

    ifstream BH("BH-ejemplo2.txt"); // para leer del fichero
    string linea; string cadena; string basura; // para lectura del contenido del fichero

    string objetivo;
    int numHechos; BH >> numHechos;    // leemos el numero de reglas
    BH.ignore(1, ' ');  // saltamos a la siguiente linea

    for(int i = 1; i <= numHechos; ++i){

        // Obtenemos el contenido de la linea
        getline(BH, linea); // extraemos la linea de la BC
        istringstream cadenas(linea); // obtenemos las cadenas de la linea

        Hecho hecho;

        // Obtenemos el id del hecho
        cadenas >> cadena;
        hecho.id = cadena.substr(0,cadena.size()-1);
        cout << hecho.id << " ";

        // Obtenemos el factor de certeza del hecho
        cadenas >> cadena;
        hecho.factorCerteza = stof(cadena.substr(3));
        cout << hecho.factorCerteza << endl;

    }

    // Ignoramos palabra objetivo
    BH >> basura;

    // Obtenemos el objetivo
    BH >> objetivo;
    cout << objetivo;

}

//////////////////////////////////////////////////////////////
////////////        PROGRAMA PRINCIPAL        ////////////////
//////////////////////////////////////////////////////////////

int main (void){

    list<Conocimiento> BC;
    lecturaBC();

    list<Hecho> BH;
    lecturaBH();


}
