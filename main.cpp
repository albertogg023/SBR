#include <assert.h>
#include <iostream>  // variables cin y cout
#include <sstream>  // iostream
#include <fstream>  // lectura ficheros
#include <list> // lista enlazada
#include <climits> // MAX_INT y MIN_INT
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

    BC.close();

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

    BH.close();

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

// COMPRUEBA SI UN HECHO SE ENCUENTRA EN LA BH
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
    if(fcAntecedente > 0)    // el factor de certeza del antecedene es mayor que 0
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
    if(fcAntecedente > 0)    // el factor de certeza del antecedene es mayor que 0
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
            if(abs(fcCalculada) < abs(fcReglas[i]))
                fcCalculada = (fcCalculada + fcReglas[i]) / (1 - abs(fcCalculada));
            else
                fcCalculada = (fcCalculada + fcReglas[i]) / (1 - abs(fcReglas[i]));
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

        if(reglaAplicada.modoAntecedentes == -1){    // CASO 3: la regla tiene un solo antecedente
            fcReglas[i] = aplicaCaso3(reglaAplicada, BH, out);
        }
        else{   // CASO 1: la regla tiene más de un antecedente
            if(reglaAplicada.modoAntecedentes == 0)   //  conjunciones
                fcAntecedente = aplicaCaso1Conjunciones(reglaAplicada, BH, out); // nos quedamos con el menor fc de sus antecedentes
            else   // disyunciones
                fcAntecedente = aplicaCaso1Disyunciones(reglaAplicada, BH, out); // nos quedamos con el menor fc de sus antecedentes
            // Aplicamos CASO 3
            fcReglas[i] = aplicaCaso3TrasCaso1(reglaAplicada, fcAntecedente, BH, out);
        }
    }

    // Vemos que factor de certeza debemos de devolver
    if(numReglasAplicadas > 1){ // CASO 2: se ha llegado al objetivo a través de varias ramas
         out << "Para cara calcular " << reglaAplicada.consecuente << " acumulando evidencia" << endl;
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

    if( contenida(objetivo, BH) )
        return true;

    list<Conocimiento> cjtoConflicto = equiparar(BC, objetivo);
    list<Conocimiento> reglasAplicadas = cjtoConflicto;

    while(!esVacia(cjtoConflicto)){

        r = resolver(cjtoConflicto); // devuelve una regla de cjtoConflicto y la elimina de el
        nuevasMetas = arrayStringsDinamicoToList(r.antecedentes);   // obtenemos los antecedentes de la regla
        verificado = true;

        while(!esVacia(nuevasMetas) && verificado){
            nuevaMeta = popMeta(nuevasMetas);
            verificado = verificar(BC, BH, nuevaMeta, out);
        }

        if(verificado && esVacia(cjtoConflicto)){
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


int main (int argc, char * argv[]){

    list<Conocimiento> BC;
    lecturaBC("BC-ejercicio4.txt", BC);

    string objetivo;
    list<Hecho> BH;
    lecturaBH("BH-ejercicio4.txt", BH, objetivo);
    ofstream out("salida-ejercicio4.txt");

    if(verificar(BC, BH, objetivo, out))
        out << "EXITO, FACTOR DE CERTEZA DE " << objetivo << " es " << getFactorCerteza(BH, objetivo) << endl;
    else
        out << "FRACASO" << endl;

    out << endl << endl << endl;
    out << "--------------------------------------------------------------------------------------------";
    out << endl << endl << endl << endl << "FACTORES DE CERTEZA DE LOS HECHOS:" << endl;
    list<Hecho>::iterator it = BH.begin();
    while(it != BH.end()){ // recorremos BH
        out << "\t" << (*it).id << ": " << (*it).factorCerteza << endl;
        ++it;
    }

    liberarMemoriaDinamica(BC);
    out.close();
}
