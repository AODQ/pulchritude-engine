#include <pulchritude/shape.h>

#include <pulchritude/error.h>
#include <pulchritude/math.h>

#include <vector>

#include <pmp/algorithms/shapes.h>
#include <pmp/algorithms/triangulation.h>

namespace pint {

PuleBuffer toBuffer(pmp::SurfaceMesh & mesh) {
  pmp::triangulate(mesh);
  PuleBuffer buffer = puleBufferCreate(puleAllocateDefault());
  puleBufferResize(&buffer, mesh.n_faces() * 3 * sizeof(float)*3);
  size_t i = 0;
  for (auto const & face : mesh.faces()) {
    for (auto const & vertex : mesh.vertices(face)) {
      auto const & point = mesh.position(vertex);
      ((float *)buffer.data)[i ++] = point.data()[0];
      ((float *)buffer.data)[i ++] = point.data()[1];
      ((float *)buffer.data)[i ++] = point.data()[2];
    }
  }
  return buffer;
}

} // namespace pint

extern "C" {

PuleBuffer puleShapeCreateTetrahedron() {
  pmp::SurfaceMesh mesh = pmp::tetrahedron();
  PuleBuffer buffer = pint::toBuffer(mesh);
  return buffer;
}

PuleBuffer puleShapeCreateHexahedron() {
  pmp::SurfaceMesh mesh = pmp::hexahedron();
  PuleBuffer buffer = pint::toBuffer(mesh);
  return buffer;
}

PuleBuffer puleShapeCreateIcosphere(size_t n) {
  pmp::SurfaceMesh mesh = pmp::icosphere();
  PuleBuffer buffer = pint::toBuffer(mesh);
  return buffer;
}

} // extern C
