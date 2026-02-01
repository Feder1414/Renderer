//
// Created by USUARIO on 1/24/2026.
//

#include "UuidGenerator.h"
#include <random>
std::array<int, std::mt19937::state_size> UuidGenerator::m_seedData{};
std::mt19937 UuidGenerator::m_randomGenerator{};
std::optional<uuids::uuid_random_generator> UuidGenerator::m_uuidRandomGenerator;

void UuidGenerator::InitializeRandomEngine()
{
    std::random_device rd;
    m_seedData = std::array<int, std::mt19937::state_size>{};
    std::generate(std::begin(m_seedData), std::end(m_seedData), std::ref(rd));
    std::seed_seq seq(std::begin(m_seedData), std::end(m_seedData));
    m_randomGenerator = std::mt19937(seq);
    m_uuidRandomGenerator = uuids::uuid_random_generator(m_randomGenerator);
}

uuids::uuid UuidGenerator::GenerateUUID()
{
    if (!m_uuidRandomGenerator.has_value())
    {
        InitializeRandomEngine();
    }
    return m_uuidRandomGenerator.value().operator()();
}
