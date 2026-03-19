//
// Created by USUARIO on 2/18/2026.
//

#ifndef GRAFICOS_GLOBALENUMMETADATA_H
#define GRAFICOS_GLOBALENUMMETADATA_H
#include <memory>

#include "ComponentProperty.h"


class GlobalEnumMetadata
{
    static std::unordered_map<std::string, std::unique_ptr<EnumMetadata>> m_glolbalEnumMetadata;

public:
    static void AddEnumMetadata(const std::string& enumName, std::unique_ptr<EnumMetadata> enumMetadata)
    {
        if (!m_glolbalEnumMetadata.contains(enumName))
        {
            m_glolbalEnumMetadata.emplace(enumName, std::move(enumMetadata));
        }
    }

    static EnumMetadata* GetEnumMetadata(const std::string& enumName)
    {
        if (m_glolbalEnumMetadata.contains(enumName))
        {
            return m_glolbalEnumMetadata[enumName].get();
        }
        return nullptr;
    }
};


#endif //GRAFICOS_GLOBALENUMMETADATA_H
