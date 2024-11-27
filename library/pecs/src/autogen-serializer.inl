/* auto generated file pecs */
/* include this in src file once at bottom */

    static PuleDsStructField const pulePecsComponent_Transform2D_fields[] = {
      {
        .dt = PuleDt_f32v2,
        .fieldByteOffset = offsetof(PulePecsComponent_Transform2D, origin),
        .fieldCount = 1,
      },
      {
        .dt = PuleDt_f32,
        .fieldByteOffset = offsetof(PulePecsComponent_Transform2D, rotation),
        .fieldCount = 1,
      },
      {
        .dt = PuleDt_f32v2,
        .fieldByteOffset = offsetof(PulePecsComponent_Transform2D, scale),
        .fieldCount = 1,
      },
      { PuleDt_ptr, 0, 0 },
    };
extern "C" {
  void pulePecsComponent_Transform2DSerialize(void const * const result, PuleDsValue const dsValue) {
    puleDsStructSerialize(dsValue, puleAllocateDefault(), pulePecsComponent_Transform2D_fields, result);
  }
} // extern C
extern "C" {
void pulePecsComponent_Transform2DDeserialize(void * const data, PuleDsValue const dsValue) {
  puleDsStructDeserialize(dsValue, pulePecsComponent_Transform2D_fields, data);
}
} // extern C
    static PuleDsStructField const pulePecsComponent_Camera_fields[] = {
      {
        .dt = PuleDt_f32,
        .fieldByteOffset = offsetof(PulePecsComponent_Camera, fov),
        .fieldCount = 1,
      },
      {
        .dt = PuleDt_f32,
        .fieldByteOffset = offsetof(PulePecsComponent_Camera, near),
        .fieldCount = 1,
      },
      {
        .dt = PuleDt_f32,
        .fieldByteOffset = offsetof(PulePecsComponent_Camera, far),
        .fieldCount = 1,
      },
      { PuleDt_ptr, 0, 0 },
    };
extern "C" {
  void pulePecsComponent_CameraSerialize(void const * const result, PuleDsValue const dsValue) {
    puleDsStructSerialize(dsValue, puleAllocateDefault(), pulePecsComponent_Camera_fields, result);
  }
} // extern C
extern "C" {
void pulePecsComponent_CameraDeserialize(void * const data, PuleDsValue const dsValue) {
  puleDsStructDeserialize(dsValue, pulePecsComponent_Camera_fields, data);
}
} // extern C
