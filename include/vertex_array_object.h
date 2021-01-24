#pragma once

#include "utility.h"

namespace mandel::gl
{

    class verArrObj : public __baseObject
    {
    public:

        verArrObj();
        ~verArrObj();

        void m_create();

        void m_bind() const override;
        void m_unbind() const override;
    };

}