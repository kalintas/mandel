#include "vertex_array_object.hpp"

namespace mandel::gl {

verArrObj::verArrObj() {}

verArrObj::~verArrObj() {
    GLCALL(glDeleteVertexArrays(1, &m_id));
}

void verArrObj::m_create() {
    GLCALL(glGenVertexArrays(1, &m_id));
}

void verArrObj::m_bind() const {
    GLCALL(glBindVertexArray(m_id));
}
void verArrObj::m_unbind() const {
    GLCALL(glBindVertexArray(0));
}
}  // namespace mandel::gl