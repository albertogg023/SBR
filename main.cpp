#include <cassert> // assert
#include <cstdio>  // operaciones input/output, etc
#include <cstdlib> // operaciones dinamicas, etc
#include <iostream>  // cin y cout, strings, etc
#include <sstream>  // iostream
#include <fstream>  // lectura y escritura ficheros
#include <list> // lista enlazada
#include <cstring>  // funciones string
#include <cmath>    // abs
#include <climits> // MAX_INT y MIN_INT
using namespace std;

#define MODO_UN_LITERAL -1
#define MODO_CONJUNCION 0
#define MODO_DISYUNCION 1
#define CADENA_INDICA_CONJUNCION "y"
#define CADENA_INDICA_DISYUNCION "o"


//////////////////////////////////////////////////////////////
////////////        TIPOS DE DATOS            ////////////////
//////////////////////////////////////////////////////////////

typedef struct {
    string id;  // identificador de la regla
    int modoAntecedentes;   // indica si un literal solo, o varios y si son separados por conjunciones o disyunciones
    string * antecedentes;  // array de antecedentes de la regla en memoria dinamica
    string consecuente; // consecuente de la regla
    double factorCerteza;    // factor de certeza de la regla
} Conocimiento;

typedef struct {
    string id;  // identificador del hecho
    double factorCerteza;    // factor de certeza del hecho
} Hecho;


//////////////////////////////////////////////////////////////
/////////     FUNCIONES DE LECTURA DE BC Y BH        /////////
//////////////////////////////////////////////////////////////

// LECTURA DE LA BASE DE CONOCIMIENTO DE UN FICHERO
void lecturaBC(string file, list<Conocimiento> & listaConocimientos){

    ifstream BC(file.c_str()); // para leer del fichero
    string linea; string cadena; string basura; char cBasura; // para leer el contenido del fichero
    list<string> listaAntecentes;   // para guardar provisionalmente los antecedentes de la regla actual
    int numReglas; BC >> numReglas;    // leemos el numero de reglas
    BC.ignore(1, ' ');  // saltamos a la siguiente linea

    for(int i = 1; i <= numReglas; i++){

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
        int modoAntecedentes = MODO_UN_LITERAL;  // suponemos inicialmente que solo hay un literal
        while(cadenas >> cadena && cadena != "Entonces" ){
            if(cadena == CADENA_INDICA_CONJUNCION)
                modoAntecedentes = MODO_CONJUNCION;
            else if(cadena == CADENA_INDICA_DISYUNCION)
                modoAntecedentes = MODO_DISYUNCION;
            else
                listaAntecentes.push_back(cadena);
        }
        conocimiento.modoAntecedentes = modoAntecedentes;
        conocimiento.antecedentes = new string[listaAntecentes.size()+1];   // reservamos memoria para guardar los antecedentes y la marca de fín
        int indAntecedentes = 0;
        list<string>::iterator it = listaAntecentes.begin();
        while(it != listaAntecentes.end()){ // insertamos los antecedentes de la regla
            conocimiento.antecedentes[indAntecedentes++] = *it;
            ++it;
        } conocimiento.antecedentes[indAntecedentes] = "\0"; // insertamos marca de fín

        // Lectura consecuente
        cadenas >> cadena;
        conocimiento.consecuente = cadena.substr(0, cadena.size()-1);

        // Lectura FC
        cadenas >> cBasura;
        cadenas >> cBasura;
        cadenas >> cBasura;
        cadenas >> conocimiento.factorCerteza;
        listaConocimientos.push_back(conocimiento); // insertamos el conocimiento en la colección

    }

    BC.close(); // cerramos el fichero

}

// LECTURA DE LA BASE DE HECHOS DE UN FICHERO
void lecturaBH(string file, list<Hecho> & listaHechos, string & objetivo){

    ifstream BH(file.c_str()); // para leer del fichero
    string linea; string cadena; string basura; char cBasura; // para lectura del contenido del fichero

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
        cadenas >> cBasura;
        cadenas >> cBasura;
        cadenas >> cBasura;
        cadenas >> hecho.factorCerteza;

        listaHechos.push_back(hecho);   // insertamos el hecho en la colección

    }

    // Ignoramos palabra objetivo
    BH >> basura;

    // Obtenemos el objetivo
    BH >> objetivo;

    BH.close(); // cerramos el fichero

}


//////////////////////////////////////////////////////////////
/////////          FUNCIONES AUXILIARES              /////////
//////////////////////////////////////////////////////////////

// LIBERAR TODA LA MEMORIA DINÁMICA RESERVADA DURANTE LA EJECUCIÓN DEL PROGRAMA
void liberarMemoriaDinamica(list<Conocimiento> & BC){
    list<Conocimiento>::iterator it = BC.begin();
    while(it != BC.end()){
        delete[] (*it).antecedentes;
        ++it;
    }
}

// DEVUELVE SI LA COLECCION PASADA COMO PARÁMETRO ES VACIA O NO
bool esVacia(list<Conocimiento> & coleccion){
    return coleccion.empty();
}

// DEVUELVE SI LA COLECCION PASADA COMO PARÁMETRO ES VACIA O NO
bool esVacia(list<string> & coleccion){
    return coleccion.empty();
}

// DEVUELVE UNA LISTA DE STRINGS A PARTIR DE UN ARRAY QUE TENGA MARCA DE FÍN
list<string> arrayStringsDinamicoToList(string * arrayStrings){
    list<string> listaStrings;
    int i = 0;
    while(arrayStrings[i] != "\0"){
        listaStrings.push_back(arrayStrings[i]);
        ++i;
    }
    return listaStrings;
}

// COMPRUEBA SI UN HECHO SE ENCUENTRA EN LA BH
bool contenida(string meta, list<Hecho> & BH){
    list<Hecho>::iterator it = BH.begin();
    while(it != BH.end()){ // recorremos BH
        if( (*it).id == meta )  // comprobamos si la meta es el hecho actual
            return true;    // lo hemos encontrado
        ++it;
    }
    return false;   // hemos acabado la búsqueda y no lo hemos encontrado
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
    assert(!cjtoConflicto.empty()); // comprobamos que efectivamente no es vacío el cjto
    Conocimiento conocimiento = cjtoConflicto.front();  // devolvemos el primer elemento del cjto
    cjtoConflicto.pop_front();  // lo eliminamos del cjto
    return conocimiento;
}

// OBTIENE UNA META DEL CJTO NUEVASMETAS Y LA ELIMINA DE ESTE
string popMeta(list<string> & metas){
    assert(!metas.empty()); // comprobamos que efectivamente no es vacío el cjto
    string meta = metas.front();  // devolvemos el primer elemento del cjto
    metas.pop_front();  // lo eliminamos del cjto
    return meta;
}

// ANADE UN HECHO A LA BASE DE HECHOS
void anadirHecho(list<Hecho> & BH, Hecho hecho){
    BH.push_back(hecho);
}

// OBTIENE EL FACTOR DE CERTEZA DE UN HECHO QUE SE ENCUENTRA EN LA BH
double getFactorCerteza(list<Hecho> & BH, string id){
    list<Hecho>::iterator it = BH.begin();
    while(it != BH.end()){ // recorremos BH
        if( (*it).id == id )
            return (*it).factorCerteza;
        ++it;
    }
    return INT_MIN; // si no se encuentra en la base de hechos
}


//////////////////////////////////////////////////////////////
/////////  CASOS PARA CALCULAR FACTORES DE CERTEZA   /////////
//////////////////////////////////////////////////////////////

double aplicaCaso3(Conocimiento & reglaAplicada, list<Hecho> & BH, ofstream & out){
    double fcCalculado;
    double fcAntecedente = getFactorCerteza(BH, reglaAplicada.antecedentes[0]);
    out << "INICIO CASO 3" << endl;
    out << "Factor de certeza de la regla es " << reglaAplicada.factorCerteza << endl;
    out << "Factor de certeza del antecedente " << reglaAplicada.antecedentes[0] << " es: " << fcAntecedente << endl;
    if(fcAntecedente > 0)    // el factor de certeza del antecedente es mayor que 0
        fcCalculado = reglaAplicada.factorCerteza*fcAntecedente;
    else    // en caso contrario
        fcCalculado = 0;
    out << "Factor de certeza calculado: " << fcCalculado << endl;
    out << "FIN CASO 3" << endl;
    return fcCalculado;
}

double aplicaCaso3TrasCaso1(Conocimiento & reglaAplicada, double fcAntecedente, list<Hecho> & BH, ofstream & out){
    double fcCalculado;
    out << "INICIO CASO 3" << endl;
    out << "Factor de certeza de la regla es " << reglaAplicada.factorCerteza << endl;
    out << "Factor de certeza del antecedente " << reglaAplicada.antecedentes[0] << " es: " << fcAntecedente << endl;
    if(fcAntecedente > 0)    // el factor de certeza del antecedente es mayor que 0
        fcCalculado = reglaAplicada.factorCerteza*fcAntecedente;
    else    // en caso contrario
        fcCalculado = 0;
    out << "Factor de certeza calculado: " << fcCalculado << endl;
    out << "FIN CASO 3" << endl;
    return fcCalculado;
}

double aplicaCaso1Conjunciones(Conocimiento & reglaAplicada, list<Hecho> & BH, ofstream & out){
    double fcAntecedente = getFactorCerteza(BH, reglaAplicada.antecedentes[0]);
    double fcCalculado = fcAntecedente;
    out << "INICIO CASO 1" << endl;
    out << "Antecedentes separados con conjunciones" << endl;
    out << "Antecedente de la regla " << reglaAplicada.antecedentes[0] << " con fc: " << fcAntecedente << endl;
    unsigned int j = 1;
    while(reglaAplicada.antecedentes[j] != "\0"){
        fcAntecedente = getFactorCerteza(BH, reglaAplicada.antecedentes[j]);
        out << "Antecedente de la regla " << reglaAplicada.antecedentes[j] << " con fc: " << fcAntecedente << endl;
        if( fcAntecedente < fcCalculado){
            fcCalculado = fcAntecedente;
        }
        ++j;
    }
    out << "Factor de certeza del antecedente seleccionado es: " << fcCalculado << endl << "FIN CASO 1" << endl;
    return fcCalculado;
}

double aplicaCaso1Disyunciones(Conocimiento & reglaAplicada, list<Hecho> & BH, ofstream & out){
    double fcAntecedente = getFactorCerteza(BH, reglaAplicada.antecedentes[0]);
    double fcCalculado = fcAntecedente;
    out << "INICIO CASO 1" << endl;
    out << "Antecedentes separados con disyunciones" << endl;
    out << "Antecedente de la regla " << reglaAplicada.antecedentes[0] << " con fc: " << fcAntecedente << endl;
    unsigned int j = 1;
    while(reglaAplicada.antecedentes[j] != "\0"){
        fcAntecedente = getFactorCerteza(BH, reglaAplicada.antecedentes[j]);
        out << "Antecedente de la regla " << reglaAplicada.antecedentes[j] << " con fc: " << fcAntecedente << endl;
        if( fcAntecedente > fcCalculado){
            fcCalculado = fcAntecedente;
        }
        ++j;
    }
    out << "Factor de certeza del antecedente seleccionado es: " << fcCalculado << endl << "FIN CASO 1" << endl;
    return fcCalculado;
}

double aplicaCaso2(double * fcReglas, unsigned int numReglasAplicadas, ofstream & out){
    out << "INICIO CASO 2" << endl;
    double fcCalculada = fcReglas[0];
    for(unsigned int i = 1; i < numReglasAplicadas; i++){
        if(fcCalculada >= 0 && fcReglas[i] >= 0)
            fcCalculada = fcCalculada + fcReglas[i]*(1-fcCalculada);
        else if(fcCalculada <= 0 && fcReglas[i] <= 0)
            fcCalculada = fcCalculada + fcReglas[i]*(1+fcCalculada);
        else{
            if(fabs(fcCalculada) < fabs(fcReglas[i]))
                fcCalculada = (fcCalculada + fcReglas[i]) / (1 - fabs(fcCalculada));
            else
                fcCalculada = (fcCalculada + fcReglas[i]) / (1 - fabs(fcReglas[i]));
        }
    }
    out << "Factor de certeza calculado: " << fcCalculada << endl << "FIN CASO 2" << endl;
    return fcCalculada;

}


//////////////////////////////////////////////////////////////
/////////     FUNCION CALCULA FACTOR DE CERTEZA      /////////
//////////////////////////////////////////////////////////////
double calcularFactorCerteza(string objetivo, list<Conocimiento> & cjtoReglasAplicadas, list<Hecho> & BH, ofstream & out){
    double fcAdquirida;
    double fcAntecedente;
    unsigned int numReglasAplicadas = cjtoReglasAplicadas.size();
    double * fcReglas = new double[numReglasAplicadas];
    Conocimiento reglaAplicada;

    for(unsigned int i = 0; i<numReglasAplicadas; i++){

        reglaAplicada = resolver(cjtoReglasAplicadas);  // obtenemos y eliminamos una regla del cjto ReglasAplicadas
        out << "Para calcular " << reglaAplicada.consecuente << " con " << reglaAplicada.id << endl;

        if(reglaAplicada.modoAntecedentes == MODO_UN_LITERAL){    // CASO 3: la regla tiene un solo antecedente
            fcReglas[i] = aplicaCaso3(reglaAplicada, BH, out);
        }
        else{   // CASO 1: la regla tiene más de un antecedente
            if(reglaAplicada.modoAntecedentes == MODO_CONJUNCION)   //  conjunciones
                fcAntecedente = aplicaCaso1Conjunciones(reglaAplicada, BH, out); // nos quedamos con el menor fc de sus antecedentes
            else   // disyunciones
                fcAntecedente = aplicaCaso1Disyunciones(reglaAplicada, BH, out); // nos quedamos con el menor fc de sus antecedentes
            // Aplicamos CASO 3
            fcReglas[i] = aplicaCaso3TrasCaso1(reglaAplicada, fcAntecedente, BH, out);
        }
    }

    // Vemos que factor de certeza debemos de devolver
    if(numReglasAplicadas > 1){ // CASO 2: se ha llegado al objetivo a través de varias ramas
         out << "Para calcular " << reglaAplicada.consecuente << " acumulando evidencia" << endl;
        fcAdquirida = aplicaCaso2(fcReglas, numReglasAplicadas, out);
    }else   // se ha llegado al objetivo a través de una rama solo
        fcAdquirida = fcReglas[0];
    out << endl << endl;
    return fcAdquirida;

}


//////////////////////////////////////////////////////////////
/////////            FUNCION VERIFICA                /////////
//////////////////////////////////////////////////////////////
bool verificar(list<Conocimiento> & BC, list<Hecho> & BH, string objetivo, ofstream & out){

    Conocimiento r;
    string nuevaMeta;
    list<string> nuevasMetas;
    bool verificado = false;

    if(contenida(objetivo, BH))
        return true;

    list<Conocimiento> cjtoConflicto = equiparar(BC, objetivo);
    list<Conocimiento> reglasAplicadas = cjtoConflicto;

    while(!esVacia(cjtoConflicto)){

        r = resolver(cjtoConflicto); // devuelve una regla de cjtoConflicto y la elimina de él
        nuevasMetas = arrayStringsDinamicoToList(r.antecedentes);   // obtenemos los antecedentes de la regla
        verificado = true;

        while(!esVacia(nuevasMetas) && verificado){
            nuevaMeta = popMeta(nuevasMetas); // devuelve una meta de nuevasMetas y la elimina de él
            verificado = verificar(BC, BH, nuevaMeta, out);
        }

        if(verificado && esVacia(cjtoConflicto)){   // si se ha podido verificar y lo hemos hecho por todas sus ramas
            Hecho meta; meta.id = objetivo;
            meta.factorCerteza = calcularFactorCerteza(objetivo, reglasAplicadas, BH, out);
            anadirHecho(BH, meta);
        }

    }

    return verificado;

}


//////////////////////////////////////////////////////////////
////////////        PROGRAMA PRINCIPAL        ////////////////
//////////////////////////////////////////////////////////////

int main (int argc, char ** argv){

    // Leemos la base de conocimiento
    list<Conocimiento> BC;
    lecturaBC(argv[1], BC);

    // Leemos la base de hechos
    string objetivo;
    list<Hecho> BH;
    lecturaBH(argv[2], BH, objetivo);

    // Creamos un fichero de salida para redirigir la salida
    string nombreBC(argv[1]);
    unsigned int posPunto = nombreBC.rfind(".");
    string nombreBH(argv[2]);
    posPunto = nombreBH.rfind(".");
    string nombreFicheroSalida = "salida-" + nombreBC.substr(0, posPunto) + "-" + nombreBH.substr(0, posPunto) + ".txt";
    ofstream out(nombreFicheroSalida.c_str(), ios::out);

    // Mostramos información al usuario sobre los ficheros usados
    out << "Base de conocimiento usada: " << nombreBC << endl  << endl;
    out << "Base de hechos utilizada: " << nombreBH << endl << endl;
    out << "Objetivo a calcular: " << objetivo << endl;
    out << endl << endl << endl;
    out << "--------------------------------------------------------------------------------------------";
    out << endl << endl << endl << endl;

    // Calculamos la red de inferencia
    if(verificar(BC, BH, objetivo, out))
        out << "EXITO, FACTOR DE CERTEZA DE " << objetivo << " es " << getFactorCerteza(BH, objetivo) << endl;
    else
        out << "FRACASO" << endl;

    liberarMemoriaDinamica(BC); // liberamos la memoria dinámica reservada
    out.close();    // cerramos el fichero utilizado para redirigir la salida

}
