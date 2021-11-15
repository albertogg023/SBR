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
    char id;
    float factor;
} Hecho;


//////////////////////////////////////////////////////////////
////////////     FUNCIONES DEL PROGRAMA       ////////////////
//////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////
////////////        PROGRAMA PRINCIPAL        ////////////////
//////////////////////////////////////////////////////////////

int main (void){

    // HACEMOS LO NECESARIO PARA PODER LEER EL FICHERO DE BC
    ifstream BC("BC-ejemplo2.txt");
    string linea; string cadena; string basura;

    // LECTURA DE BC
    string idString;
    list<string> listaAntecentes;
    string consecuenteString;
    string factorCertezaString;
    int numReglas; BC >> numReglas;    // leemos el numero de reglas
    BC.ignore(1, ' ');  // saltamos a la siguiente linea
    //getline(BC, linea); // extraemos la linea de la BC
    for(int i = 1; i <= numReglas; ++i){

        // Obtenemos el contenido de la linea
        getline(BC, linea); // extraemos la linea de la BC
        istringstream cadenas(linea); // obtenemos las cadenas de la linea

        // Creamos la estructura Conocimiento
        Conocimiento con;

        // Lectura idRegla:
        cadenas >> idString;    // extraemos el id de la regla con ':' incluidos
        con.id = idString.substr(0, idString.size()-1);
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
        cadenas >> consecuenteString;
        con.consecuente = consecuenteString.substr(0, consecuenteString.size()-1);
        cout << con.consecuente << " ";

        // Lectura FC
        cadenas >> factorCertezaString;
        con.factorCerteza = stof(factorCertezaString.substr(3));
        cout << con.factorCerteza;
        cout << endl;
    }

    // LECTURA DE LA BASE DE HECHOS
}
