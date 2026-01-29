//
// Created by USUARIO on 1/24/2026.
//

#ifndef GRAFICOS_UUIDGENEREATOR_H
#define GRAFICOS_UUIDGENEREATOR_H
#include <random>

#include "uuid.h"


class UuidGenerator
{
private:
    static std::array<int, std::mt19937::state_size> m_seedData;
    static std::mt19937 m_randomGenerator;
    static std::optional<uuids::uuid_random_generator> m_uuidRandomGenerator;
    static void InitializeRandomEngine();

public:
    static uuids::uuid GenerateUUID();
};


#endif //GRAFICOS_UUIDGENEREATOR_H
