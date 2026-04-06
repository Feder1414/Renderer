//
// Created by USUARIO on 3/23/2026.
//

#ifndef GRAFICOS_ENUMUTILS_H
#define GRAFICOS_ENUMUTILS_H

namespace EnumUtils
{
    inline int BitMaskOrOperator(int lo, int ro)
    {
        return lo | ro;
    }

    inline int BitMaskAndOperator(int lo, int ro)
    {
        return lo & ro;
    }
}


#endif //GRAFICOS_ENUMUTILS_H
