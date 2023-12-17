//
// Created by stefan on 10/12/2023.
//

// IMPORTANTE cuando en la práctica pide que se busque por id se refiere al iata

#include "VuelaFlightDoble2.h"
#include <iostream>
#include <fstream>
#include <string>

VuelaFlightDoble2::VuelaFlightDoble2(): airports(THashAeropuerto(10)) {

}

/**
 * @brief   Constructor parametrizado de la clase VuelaFlight
 * @details REcibe como parámetros tres ifstrem que son el contenido de del fichero csv del que queremos leer los datos de vuelaflight. Esta función asigna a todas las rutas su compañia, aeropuerto de origen y destino correspondientes. También asigna a todas las Aerolineas todas las rutas que realizan.
 * @param[in]fichAirports   Contenido del fichero de aeropuertos en formato csv:  id;iata;tipo;nombre;latitud;longitud;continente;iso_pais
 * @param[in]fichAerolines  Contenido del fichero de aerolineas en formato csv:   id,icao, nombre, pais, activo
 * @param[in]fichRoutes     Contenido del fichero de rutas en formato csv:        aerolinea, origin, destination
 * @post    El objeto es creado con todos los datos leídos del fichero correspondiente
 * */
VuelaFlightDoble2::VuelaFlightDoble2(std::ifstream &fichAirports, std::ifstream &fichAerolines, std::ifstream &fichRoutes, std::ifstream &fichFlights, unsigned long int maxAirpotsToInsert, float lambda): airports(THashAeropuerto(maxAirpotsToInsert, lambda)) {
    std::string fila;
    if(fichAirports.good()){
        Aeropuerto aeropuerto;
        UTM  utm;

        unsigned int id;
        std::string iata;
        std::string tipo;
        std::string nombre;
        std::string continente;
        std::string iso_pais;
        float latitud, longitud;

        //Descartamos la primera línea del documento csv, ya que no contiene ningún aeropuerto.
        std::string lineaBasura;
        std::getline(fichAirports,lineaBasura);

        while (std::getline(fichAirports,fila)){
            if(fila != ""){
                std::stringstream columnas(fila);
                columnas.str(fila);
                //formato de fila: id;iata;tipo;nombre;latitud;longitud;continente;iso_pais
                columnas>>id;
                columnas.ignore( ); //Hacemos esto para evitar la conversión de "id" a string
                getline(columnas,iata,';'); //Leemos caracteres hasta encontrar y omitir ';'
                getline(columnas,tipo,';');
                getline(columnas,nombre,';');
                columnas>>latitud;
                columnas.ignore( );
                columnas>>longitud;
                columnas.ignore( );
                getline(columnas,continente,';');
                getline(columnas,iso_pais,';');

                aeropuerto.setId(id);
                aeropuerto.setIata(iata);
                aeropuerto.setTipo(tipo);
                aeropuerto.setNombre(nombre);
                utm.setLatitud(latitud);
                utm.setLongitud(longitud);
                aeropuerto.setPosicion(utm);
                aeropuerto.setContinente(continente);
                aeropuerto.setIsoPais(iso_pais);
                this->addAeropuerto(aeropuerto);

                fila = "";
                columnas.clear();
            }
        }
        fichAirports.close();

    } else {
        std::cout << "Error de apertura en archivo" << std::endl;
    }

    if(fichAerolines.good()){
        Aerolinea aerolinea;
        unsigned int id;
        std::string icao;
        std::string nombre;
        std::string pais;
        std::string activo;
        //Descartamos la primera línea del documento csv, ya que no contiene ninguna aerolinea
        std::string lineaBasura;
        std::getline(fichAerolines,lineaBasura);

        while(std::getline(fichAerolines,fila)){
            if(fila != ""){
                std::stringstream columnas(fila);
                columnas.str(fila);
                //formato de aerolineas: id,icao, nombre, pais, activo
                columnas>>id;
                columnas.ignore( );
                std::getline(columnas,icao,';');
                std::getline(columnas,nombre,';');
                std::getline(columnas,pais,';');
                std::getline(columnas,activo,';');

                aerolinea.setId(id);
                aerolinea.setIcao(icao);
                aerolinea.setNombre(nombre);
                aerolinea.setPais(pais);
                if(activo == "Y"){
                    aerolinea.setActivo(true);
                }else{
                    aerolinea.setActivo(false);
                }
                aerolines.insert(std::pair<std::string, Aerolinea>(aerolinea.getIcao(), aerolinea));

                fila = "";
                columnas.clear();
            }
        }
        fichAerolines.close();
    } else {
        std::cout << "Error de apertura en archivo" << std::endl;
    }

    int numAerolines = aerolines.size();

    if(fichRoutes.good()){
        Ruta ruta;
        std::string aerolinea;
        std::string idOrigen;
        std::string idDestino;
        //Descartamos la primera línea del documento csv, ya que no contiene ningúna ruta.
        std::string lineaBasura;
        std::getline(fichRoutes,lineaBasura);

        while (std::getline(fichRoutes,fila)){
            if(fila != ""){
                std::stringstream columnas(fila);
                columnas.str(fila);
                //formato de fila2: aerolinea, origin, destination
                getline(columnas, aerolinea, ';'); //leemos caracteres hasta encontrar y omitir ';'
                getline(columnas,idOrigen,';');
                getline(columnas,idDestino,';');
                // Se inserta la ruta en la lista enlazada
                Ruta *rutaAgnadida = this->addNewRute(idOrigen,idDestino,aerolinea);
                if (rutaAgnadida){
                    Aerolinea *aerolineaRuta = &rutaAgnadida->getCompany();
                    aerolineaRuta->linkAerolRuta(rutaAgnadida);
                }
                fila = "";
                columnas.clear();
            }
        }
        fichRoutes.close();
    } else {
        std::cout << "Error de apertura en archivo" << std::endl;
    }


    if(fichFlights.good()){

        Vuelo flight;
        std::string flightnum;
        std::string iataDep;
        std::string iataArr;
        std::string plane;
        std::string datmeteo;
        std::string datDate;
        int dia;
        int mes;
        int agno;
        //Descartamos la primera línea del documento csv, ya que no contiene ningún vuelo.
        std::string lineaBasura;
        std::getline(fichFlights, lineaBasura);

        while (std::getline(fichFlights, fila)){
            if(fila != ""){
                std::stringstream columnas(fila);
                columnas.str(fila);
                //formato de infovuelos_v11.csv: flightNumber;dep_airport_code;arr_airport_code;plane;dep_weather_desc;dep_date
                std::getline(columnas,flightnum,';');
                std::getline(columnas,iataDep,';');
                std::getline(columnas,iataArr,';');
                std::getline(columnas,plane,';');
                std::getline(columnas,datmeteo,';');
                std::getline(columnas,datDate,';');
                std::istringstream date(datDate);
                char delimiter;
                date >>dia >>delimiter >>mes >>delimiter >>agno;
                Fecha fecha(dia, mes, agno);
                registrarVuelo(flightnum, iataDep, iataArr, plane, datmeteo, fecha);
            }
        }
    } else {
        std::cout << "Error de apertura en archivo" << std::endl;
    }

    std::cout<<"Cantidad de aeropuertos cargados: " <<airports.getnumElementos() <<std::endl;
    std::cout<<"Cantidad de aerolineas cargadas: " <<aerolines.size() <<std::endl;
    std::cout<<"Cantidad de rutas cargadas: " <<routesOrigin.size() <<std::endl;
}


/**
 * @brief   Inserta un auropuerto nuevo en el vector dinámico de aeropuertos
 * @param[in]aeropuerto objeto aeropuerto que se quiere insertar
 * @post    Un nuevo aeropuerto es añadido
 * */
void VuelaFlightDoble2::addAeropuerto(const Aeropuerto &aeropuerto) {
    unsigned long  clave = airports.funcionHashing(aeropuerto.getIata());
    airports.insertarDoble2(clave, aeropuerto.getIata(), aeropuerto);
}


/**
 * @brief   Busca una ruta que coincida con el id del aeropuerto de origen y de destino
 * @param[in]idAerOrig  iata de l aeropuerto de origen
 * @param[in]idAerDest  iata del aeropuerto de destino
 * @return  Devuelve, si se ha encontrado, la ruta con las características exigidas
 * @return  Devuelve, si no se ha encontrado la ruta especificada, un puetero a nulo
 * */
Ruta* VuelaFlightDoble2::buscarRutasOriDes(const std::string &idAerOrig, const std::string &idAerDest) {
    std::multimap<std::string, Ruta>::iterator iteratorRuteOrig = routesOrigin.find(idAerOrig);
    std::multimap<std::string, Ruta*>::iterator iteratorRuteDest = routesDest.find(idAerDest);
    if (&iteratorRuteOrig->second == iteratorRuteDest->second){
        return iteratorRuteDest->second;
    }

    return nullptr;
}


/**
 * @brief   Devuelve una std::list de punteros a las rutas que tengan el aeropuerto origen pasado como parámetro
 * @param[in]idAerOrig  id(iata) del aeropuerto origen que se quiere busca de entre todas las rutas
 * @post    Una std::list de punteros a rutas con todas las rutas que tengan como aeropuerto de origen el pasado como parámetro es devuelta
 * */
std::list<Ruta*> VuelaFlightDoble2::buscarRutasOrigen(const std::string &idAerOrig) {
    std::list<Ruta*> listaNueva;
    std::multimap<std::string, Ruta>::iterator iteratorRoutesOrig = routesOrigin.find(idAerOrig);

    if(iteratorRoutesOrig != routesOrigin.end()){
        while (iteratorRoutesOrig->first == idAerOrig){
            listaNueva.push_back(&iteratorRoutesOrig->second);
            ++iteratorRoutesOrig;
        }
    }
    return listaNueva;
}

/**
 * @brief   Busca todos los aeropuertos del pais indicado.
 * @details Se utiliza una lista enlazada de std::list ya que tiene una inserción poco costosa.
 * @param[in]pais   Pais del que se quieren buscar los aeropuertos.
 * @post    Devuelve una lista enlazada con todos los aeropuertos del pais pasado como parámetro.
 * */
std::list<Aeropuerto*> VuelaFlightDoble2::buscarAeropuertoPais(const std::string &pais) {
    std::list<Aeropuerto*> listaNueva;

    for (int i = 0; i < airports.getTamTabla(); ++i) {
        if(airports[i].getIsoPais() == pais){
            listaNueva.push_back(&airports[i]);
        }
    }
    return listaNueva;
}


/**
 * @brief   Añade una nueva ruta a la lista enlazada de rutas
 * @details Añade la nueva ruta a la lista enlazada. Busca entre el AVL de aerolineas para enlazar la correspondiente y en el vector dinamico de aeropuertos para enlazar el aeropuerto de origen y de destino.
 * @param[in]idAerOrig  Iata del aeropuerto origen de la ruta. Nos servir'a para buscarlo entre el vector din'amico con una busqueda binaria.
 * @param[in]idAerDest  Iata del aeropuerto destino de la ruta. Nos serria para buscarlo entre el vector dinamico con una busqueda binaria.
 * @param[in]aerolin    Id de la aerolinea. Nos servira para buscar la aerolinea en el AVL y asignarla a la ruta.
 * @post    Si todos los campos han sido encontrados se añade la nueva ruta.
 * @return  De vuelve la direccion de memoria de la ruta añadida
 * */
Ruta* VuelaFlightDoble2::addNewRute(const std::string &idAerOrig, const std::string &idAerDest, const std::string &aerolin) {
    // Creo la ruta que será añadida
    Ruta rutanueva;
    // Creo los objetos necesarios para realizar las comparaciones
    Aeropuerto aeropComprobacion;

    //Recordamos que en los maps se busca por el first
    std::map<std::string, Aerolinea>::iterator iterator = aerolines.find(aerolin);
    if (iterator == aerolines.end()){ //Comprobamos que se ha encontrado el elemento. .end() devuelve la siguiente posicion al ultimo elemento almacenado en map(aerlines)
        return nullptr;
    }
    rutanueva.setCompany((iterator)->second);

    aeropComprobacion.setIata(idAerOrig);

    if(!airports.buscarDoble2(idAerOrig)){
        return nullptr;
    }
    rutanueva.setOrigin(*airports.buscarDoble2(idAerOrig));

    aeropComprobacion.setIata(idAerDest);
    if(!airports.buscarDoble2(idAerDest)){
        return nullptr;
    }
    std::multimap<std::string, Ruta*>::iterator iteratorRoutesDest = routesDest.begin();
    Ruta *rutaP = iteratorRoutesDest->second;
    rutanueva.setDestination(*airports.buscarDoble2(idAerDest));

    std::multimap<std::string, Ruta>::iterator iteratorRoutesOrigin = routesOrigin.insert(std::pair<std::string, Ruta>(rutanueva.getOrigin()->getIata(),rutanueva));
    routesDest.insert(std::pair<std::string, Ruta*>(iteratorRoutesOrigin->first,&(iteratorRoutesOrigin->second)));
    return &iteratorRoutesOrigin->second;
}

//TODO comprobar que este método funciona y revisar pregunta a balsas.
Aerolinea *VuelaFlightDoble2::buscaAerolinea(const std::string &icaoAerolinea) {
    Aerolinea *aerolinea;
    std::map<std::string, Aerolinea>::iterator iteratorAerolines = aerolines.find(icaoAerolinea);
    if(iteratorAerolines != aerolines.end()){
        aerolinea = &iteratorAerolines->second;
        return aerolinea;
    }
    return nullptr;
}

std::list<Aerolinea *> VuelaFlightDoble2::buscarAerolineasActivas() {
    std::list<Aerolinea*> listAerolines;
    for (std::map<std::string, Aerolinea>::iterator iteradorAerlines = aerolines.begin(); iteradorAerlines != aerolines.end() ; ++iteradorAerlines) {
        if(iteradorAerlines->second.getActivo()){
            listAerolines.push_back(&(iteradorAerlines->second));
        }
    }
    return listAerolines;
}

std::list<Aerolinea *> VuelaFlightDoble2::getAerolineasPais(const std::string &idPais) {
    std::list<Aerolinea*> listAerolines;
    for (std::map<std::string, Aerolinea>::iterator iteradorAerlines = aerolines.begin(); iteradorAerlines != aerolines.end() ; ++iteradorAerlines) {
        if(iteradorAerlines->second.getPais() == idPais){
            listAerolines.push_back(&(iteradorAerlines->second));
        }
    }
    return listAerolines;
}

bool VuelaFlightDoble2::registrarVuelo(const std::string flightNum, const std::string iataAeroOrig, const std::string iataAeroDest,
                                 const std::string plane, const std::string datosMeteo, const Fecha &fecha) {
    // Primero comprobamos que existe la Aerolinea que queremos enlazar
    std::multimap<std::string , Aerolinea>::iterator iteratorAerolines = aerolines.find(flightNum.substr(0,3));
    if(iteratorAerolines == aerolines.end()){
        return false;
    }

    // Comprobamos que existen los aeropuertos que queremos enlazar

    if(!airports.buscarDoble2(iataAeroOrig)){
        return false;
    }

    if(!airports.buscarDoble2(iataAeroDest)){
        return false;
    }

    Aeropuerto aeroComprobacion;
    aeroComprobacion.setIata(iataAeroOrig);
    aeroComprobacion.setIata(iataAeroDest);

    //Creamos el vuelo y cargamos todos los datos
    Vuelo newFlight;
    newFlight.setAerolinea(iteratorAerolines->second);
    newFlight.setAirportOrig(*airports.buscarDoble2(iataAeroOrig));
    newFlight.setAirportDest(*airports.buscarDoble2(iataAeroDest));
    newFlight.setFlightNumb(flightNum);
    newFlight.setPlane(plane);
    newFlight.setFecha(fecha);
    newFlight.setDatMeteo(datosMeteo);

    iteratorAerolines->second.addVuelo(newFlight);
    return true;
}


std::list<Vuelo *> VuelaFlightDoble2::buscaVuelos(const std::string &fNumber) {
    std::map<std::string , Aerolinea>::iterator iteratorAeroline = aerolines.find(fNumber.substr(0,3));
    std::list<Vuelo*> listFlights;
    if(iteratorAeroline != aerolines.end()){
        listFlights = iteratorAeroline->second.getVuelos(fNumber);
    }
    return listFlights;
}


std::list<Vuelo*> VuelaFlightDoble2::vuelosOperadosPor(const std::string &icaoAerolinea, const Fecha &fecha) {
    Aerolinea *aeroline = buscaAerolinea(icaoAerolinea);
    if (aeroline){
        return aeroline->getVuelos(fecha,fecha);
    }
    std::list<Vuelo*> listFlights;
    return listFlights;
}


std::set<std::string> VuelaFlightDoble2::buscaVuelosDestAerop(const std::string &paisOrig, const std::string &iataAeropDest) {
    std::set<std::string> setFlightDestFromCountry;
    std::multimap<std::string, Ruta*>::iterator iteratorRoutes = routesDest.find(iataAeropDest);

    if(iteratorRoutes != routesDest.end()){
        while (iteratorRoutes->first == iataAeropDest){
            if(iteratorRoutes->second->getOrigin()->getIsoPais() == paisOrig){
                std::list<Vuelo*> listFlights = iteratorRoutes->second->getFlights();
                std::list<Vuelo*>::iterator iteratorListFlights = listFlights.begin();
                while (iteratorListFlights != listFlights.end()){
                    if((*iteratorListFlights)->getAeropuertoOrigin()->getIsoPais() == paisOrig){
                        setFlightDestFromCountry.insert((*iteratorListFlights)->getFlightNumb());
                    }
                    ++iteratorListFlights;
                }
                ++iteratorRoutes;
            }
        }
    }

    return setFlightDestFromCountry;
}


std::set<Aeropuerto *> VuelaFlightDoble2::buscaAeropuertosAerolinea(const std::string &icaoAerolinea) {
    Aerolinea *aerolinea;
    std::set<Aeropuerto*> setAeropuertos;
    std::map<std::string, Aerolinea>::iterator iteratorAerolines = aerolines.find(icaoAerolinea);
    if(iteratorAerolines != aerolines.end()){
        aerolinea = &iteratorAerolines->second;
        std::multimap<std::string, Vuelo> multMapFlights = aerolinea->getVuelos();
        std::multimap<std::string, Vuelo>::iterator iteratorMultMpFlights = multMapFlights.begin();
        while (iteratorMultMpFlights != multMapFlights.end()){
            setAeropuertos.insert(iteratorMultMpFlights->second.getAeropuertoOrigin());
            setAeropuertos.insert(iteratorMultMpFlights->second.getAeropuertoDestino());
            ++iteratorMultMpFlights;
        }
    }
    return setAeropuertos;
}

Aeropuerto *VuelaFlightDoble2::buscaAeropuertoIata(const std::string &iata) {
    return airports.buscarDoble2(iata);
}

unsigned int VuelaFlightDoble2::getNumAeropuertos() const {
    return airports.getnumElementos();
}

std::list<Aeropuerto *> VuelaFlightDoble2::getAeropuertosList() {
    std::list<Aeropuerto*> listAirports;
    for (int i = 0; i < airports.getTamTabla(); ++i) {
        if (&airports[i]){
            listAirports.push_back(&airports[i]);
        }
    }
    return listAirports;
}

std::vector<Aeropuerto *> VuelaFlightDoble2::getAeropuertosVector() {
    std::vector<Aeropuerto*> vectorAirports;
    for (int i = 0; i < airports.getTamTabla(); ++i) {
        if (&airports[i]){
            vectorAirports.push_back(&airports[i]);
        }
    }
    return vectorAirports;
}

//TODO revisar este metodo
void VuelaFlightDoble2::eliminarAirport(const std::string &iata) {
    std::multimap<std::string, Ruta>::iterator iteratorRoutesOrigin = routesOrigin.find(iata);
    std::multimap<std::string, Ruta*>::iterator iteratorRoutesDest = routesDest.find(iata);
    if(iteratorRoutesOrigin != routesOrigin.end()){
        // Hacemos esto porque en los multimap los objetos con igual identificador se almacenan en posiciones contiguas de memoria
        while (iteratorRoutesOrigin->first == iata){
            iteratorRoutesOrigin->second.getCompany().bajaAeropuerto(iata);
            routesOrigin.erase(iteratorRoutesOrigin);
            routesDest.erase(iteratorRoutesDest);
            ++iteratorRoutesOrigin;
            ++iteratorRoutesDest;
        }
    }
    airports.borrarDoble2(iata);

}

//TODO comprobar este metodo
void VuelaFlightDoble2::eliminarAirportsInactivos() {
    for (int i = 0; i < airports.getTamTabla(); ++i) {
        Aeropuerto *airport = &airports[i];
        if(airport){
            if (airport->getTipo() == "closed"){
                airports.borrarDoble2(airport->getIata());
            }
        }
    }
}

void VuelaFlightDoble2::mostrarEstadoTabla() const {
    std::cout<<"====================================================" <<std::endl;
    std::cout<<"Estado de la tabla hash que almacena los aeropuertos" <<std::endl;
    std::cout<<"====================================================" <<std::endl;
    std::cout<<"Numero de elementos         " <<" | " <<airports.getnumElementos() <<std::endl;
    std::cout<<"====================================================" <<std::endl;
    std::cout<<"Tamaño de la tabla          " <<" | " <<airports.getTamTabla() <<std::endl;
    std::cout<<"====================================================" <<std::endl;
    std::cout<<"Numero maximo de colisiones " <<" | " <<airports.getMaxColisiones() <<std::endl;
    std::cout<<"====================================================" <<std::endl;
    std::cout<<"Num. de veces que han habido" <<" | " <<std::endl;
    std::cout<<"10 o mas colisiones         " <<" | " <<airports.getNumMax10() <<std::endl;
    std::cout<<"====================================================" <<std::endl;
    std::cout<<"Promedio de colisiones:     " <<" | " <<airports.getPromedioColisiones() <<std::endl;
    std::cout<<"====================================================" <<std::endl;
    std::cout<<"Factor de carga             " <<" | " <<airports.getFactorCarga() <<std::endl;
    std::cout<<"====================================================" <<std::endl;
    std::cout<<"Numero de redispersiones:   " <<" | " <<airports.getNumRedispersiones() <<std::endl;
    std::cout<<"====================================================" <<std::endl;
}
