//
// Created by artin on 20/7/2017.
//

#ifndef EDGEDETECTION_PATCH_H
#define EDGEDETECTION_PATCH_H


#include <string>
#include <sstream>

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

#endif //EDGEDETECTION_PATCH_H
