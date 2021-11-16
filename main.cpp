#include <stdlib.h>  // funcion exit
#include <string.h>  // funcion memset
#include <assert.h> // funcion assert
#include <iostream>  // variables cin y cout
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
/////////     FUNCIONES DE LECTURA DE BC Y BH        /////////
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
int lecturaBH(string file, list<Hecho> & listaHechos, string & objetivo){

    ifstream BH(file); // para leer del fichero
    string linea; string cadena; string basura; // para lectura del contenido del fichero

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
/////////          FUNCIONES AUXILIARES              /////////
//////////////////////////////////////////////////////////////

// LIBERAR TODA LA MEMORIA DINAMICA RESERVADA DURANTE LA EJECUCION DEL PROGRAMA
void liberarMemoriaDinamica(list<Conocimiento> & BC){
    list<Conocimiento>::iterator it = BC.begin();
    while(it != BC.end()){ // insertamos los antecedentes de la regla
        delete[] (*it).antecedentes;
        ++it;
    }
}

// DEVUELVE SI LA COLECCION PASADA COMO PARAMETRO ES VACIA O NO
bool esVacia(list<Conocimiento> & coleccion){
    return coleccion.empty();
}

// DEVUELVE SI LA COLECCION PASADA COMO PARAMETRO ES VACIA O NO
bool esVacia(list<string> & coleccion){
    return coleccion.empty();
}

// DEVUELVE UNA LISTA DE STRINGS A PARTIR DE UN ARRAY QUE TENGA MARCA DE FIN
list<string> arrayStringsDinamicoToList(string * arrayStrings){
    list<string> listaStrings;
    int i = 0;
    while(arrayStrings[i] != "\0"){
        listaStrings.push_back(arrayStrings[i]);
        ++i;
    }
    return listaStrings;
}

// COMPRUEBA SI UN HECHO SE ENCUENTRA EN BH
bool contenida(string meta, list<Hecho> & BH){
    list<Hecho>::iterator it = BH.begin();
    while(it != BH.end()){ // recorremos BH
        if( (*it).id == meta )  // comprobamos si la meta es el hecho actual
            return true;    // lo hemos encontrado
        ++it;
    }
    return false;   // hemos acabado la busqueda y no lo hemos encontrado
}

// OBTIENE LAS REGLAS DE LA BC CUYOS CONSECUENTES SEAN EL OBJETIVO
list<Conocimiento> equiparar(list<Conocimiento> & BC, string objetivo){
    list<Conocimiento> reglasCompatibles;
    list<Conocimiento>::iterator it = BC.begin();
    while(it != BC.end()){ // recorremos BC
        if( (*it).consecuente == objetivo )  // comprobamos si la meta es el consecuente de la regla actual
            reglasCompatibles.push_back(*it);    // lo insertamos en la lista de reglas compatibles
        ++it;
    }
    return reglasCompatibles;
}

// OBTIENE UNA REGLA DEL CJTO CONFLICTO Y LA ELIMINA DE ESTE
Conocimiento resolver(list<Conocimiento> & cjtoConflicto){
    assert(!cjtoConflicto.empty()); // comprobamos que efectivamente no es vacio el cjto
    Conocimiento conocimiento = cjtoConflicto.front();  // devolvemos el primer elemento del cjto
    cjtoConflicto.pop_front();  // lo eliminamos del cjto
    return conocimiento;
}

// OBTIENE UNA META DEL CJTO NUEVASMETAS Y LA ELIMINA DE ESTE
string popMeta(list<string> & metas){
    assert(!metas.empty()); // comprobamos que efectivamente no es vacio el cjto
    string meta = metas.front();  // devolvemos el primer elemento del cjto
    metas.pop_front();  // lo eliminamos del cjto
    return meta;
}

// ANADE UN HECHO A LA BASE DE HECHOS
void anadirHecho(list<Hecho> & BH, Hecho hecho){
    BH.push_back(hecho);
}


//////////////////////////////////////////////////////////////
/////////            FUNCION VERIFICA                /////////
//////////////////////////////////////////////////////////////
bool verificar(list<Conocimiento> & BC, list<Hecho> & BH, string objetivo){

    Conocimiento r;
    string nuevaMeta;
    list<string> nuevasMetas;
    bool verificado = false;

    if( contenida(objetivo, BH) )
        return true;

    list<Conocimiento> cjtoConflicto = equiparar(BC, objetivo);

    while(!esVacia(cjtoConflicto) && !verificado){

        r = resolver(cjtoConflicto); // devuelve una regla de cjtoCOnflicto y la elimina de el
        nuevasMetas = arrayStringsDinamicoToList(r.antecedentes);   // obtenemos los antecedentes de la regla
        verificado = true;

        while(!esVacia(nuevasMetas) && verificado){
            nuevaMeta = popMeta(nuevasMetas);
            verificado = verificar(BC, BH, nuevaMeta);
        }

        if(verificado){
            Hecho meta; meta.id = objetivo; meta.factorCerteza = 0;
            anadirHecho(BH, meta);
        }

    }

    return verificado;

}


//////////////////////////////////////////////////////////////
////////////        PROGRAMA PRINCIPAL        ////////////////
//////////////////////////////////////////////////////////////

int main (void){

    list<Conocimiento> BC;
    lecturaBC("BC-ejemplo2.txt", BC);

    string objetivo;
    list<Hecho> BH;
    lecturaBH("BH-ejemplo2.txt", BH, objetivo);

    if(verificar(BC, BH, objetivo))
        cout << "EXITO";
    else
        cout << "FRACASO";

    liberarMemoriaDinamica(BC);
}
