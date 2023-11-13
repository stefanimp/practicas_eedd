//
// Created by stefan on 23/10/2023.
//

#ifndef PRACTICA_1B_RUTA_H
#define PRACTICA_1B_RUTA_H

#include "Aeropuerto.h"
#include "Aerolinea.h"

class Ruta{
private:
    //Esto con punteros o sin punteros?
    Aeropuerto *origin = nullptr;
    Aeropuerto *destination = nullptr;
    Aerolinea *company = nullptr;
public:
    //Constructores y destructor
    Ruta();
    Ruta(Aerolinea &company, Aeropuerto &origin, Aeropuerto &destination);
    ~Ruta();
    //Getters y setters
    Aeropuerto& getOrigin() const;
    void setOrigin(Aeropuerto &origin);
    Aeropuerto& getDestination() const;
    void setDestination(Aeropuerto &destination);
    void setCompany(Aerolinea &company);
    Aerolinea& getCompany() const;
    bool existeAeropuerto(const std::string &iata);



};

#endif //PRACTICA_1B_RUTA_H
