open Js.Typed_array;

type t =
  | Matrix(Float32Array.t);

let createWithoutCheck = resultFloat32Arr => Matrix(resultFloat32Arr);

let createIdentityMatrix = () =>
  Float32Array.make([|
    1.,
    0.,
    0.,
    0.,
    0.,
    1.,
    0.,
    0.,
    0.,
    0.,
    1.,
    0.,
    0.,
    0.,
    0.,
    1.,
  |])
  |> createWithoutCheck;

let value = mat =>
  switch (mat) {
  | Matrix(value) => value
  };

let _getEpsilon = () => 0.000001;

let setLookAt = (eye, center, up, mat) => {
  let (eyeX, eyeY, eyeZ) = Vector.value(eye);
  let (centerX, centerY, centerZ) = Vector.value(center);
  let (upX, upY, upZ) = Vector.value(up);

  Js.Math.abs_float(eyeX -. centerX) < _getEpsilon()
  && Js.Math.abs_float(eyeY -. centerY) < _getEpsilon()
  && Js.Math.abs_float(eyeZ -. centerZ) < _getEpsilon()
    ? mat
    : {
      let z = Vector.sub(eye, center) |> Vector.normalize;

      let x = Vector.cross(up, z) |> Vector.normalize;

      let y = Vector.cross(z, x) |> Vector.normalize;

      let (x1, x2, x3) = Vector.value(x);
      let (y1, y2, y3) = Vector.value(y);
      let (z1, z2, z3) = Vector.value(z);

      let resultFloat32Arr = value(mat);

      Float32Array.unsafe_set(resultFloat32Arr, 0, x1);
      Float32Array.unsafe_set(resultFloat32Arr, 1, y1);
      Float32Array.unsafe_set(resultFloat32Arr, 2, z1);
      Float32Array.unsafe_set(resultFloat32Arr, 3, 0.);
      Float32Array.unsafe_set(resultFloat32Arr, 4, x2);
      Float32Array.unsafe_set(resultFloat32Arr, 5, y2);
      Float32Array.unsafe_set(resultFloat32Arr, 6, z2);
      Float32Array.unsafe_set(resultFloat32Arr, 7, 0.);
      Float32Array.unsafe_set(resultFloat32Arr, 8, x3);
      Float32Array.unsafe_set(resultFloat32Arr, 9, y3);
      Float32Array.unsafe_set(resultFloat32Arr, 10, z3);
      Float32Array.unsafe_set(resultFloat32Arr, 11, 0.);
      Float32Array.unsafe_set(resultFloat32Arr, 12, -. Vector.dot(x, eye));
      Float32Array.unsafe_set(resultFloat32Arr, 13, -. Vector.dot(y, eye));
      Float32Array.unsafe_set(resultFloat32Arr, 14, -. Vector.dot(z, eye));
      Float32Array.unsafe_set(resultFloat32Arr, 15, 1.);

      resultFloat32Arr |> createWithoutCheck;
    };
};

let buildPerspective = ((fovy, aspect, near, far), mat) =>
  ContractUtils.requireCheckReturnResult(
    () =>
      ContractUtils.(
        Operators.(
          test(
            LogUtils.buildAssertMessage(
              ~expect={j|frustum not be null|j},
              ~actual={j|be|j},
            ),
            () => {
              let fovy = CameraWT.Frustum.Fovy.value(fovy);

              Js.Math.sin(Js.Math._PI *. fovy /. 180. /. 2.) <>=. 0.;
            },
          )
        )
      ),
    () => {
      let fovy = CameraWT.Frustum.Fovy.value(fovy);
      let aspect = CameraWT.Frustum.Aspect.value(aspect);
      let near = CameraWT.Frustum.Near.value(near);
      let far = CameraWT.Frustum.Far.value(far);

      let fovy = Js.Math._PI *. fovy /. 180. /. 2.;
      let s = Js.Math.sin(fovy);
      let rd = 1. /. (far -. near);
      let ct = Js.Math.cos(fovy) /. s;

      let resultFloat32Arr = value(mat);

      Float32Array.unsafe_set(resultFloat32Arr, 0, ct /. aspect);
      Float32Array.unsafe_set(resultFloat32Arr, 1, 0.);
      Float32Array.unsafe_set(resultFloat32Arr, 2, 0.);
      Float32Array.unsafe_set(resultFloat32Arr, 3, 0.);
      Float32Array.unsafe_set(resultFloat32Arr, 4, 0.);
      Float32Array.unsafe_set(resultFloat32Arr, 5, ct);
      Float32Array.unsafe_set(resultFloat32Arr, 6, 0.);
      Float32Array.unsafe_set(resultFloat32Arr, 7, 0.);
      Float32Array.unsafe_set(resultFloat32Arr, 8, 0.);
      Float32Array.unsafe_set(resultFloat32Arr, 9, 0.);
      Float32Array.unsafe_set(resultFloat32Arr, 10, -. (far +. near) *. rd);
      Float32Array.unsafe_set(resultFloat32Arr, 11, -1.);
      Float32Array.unsafe_set(resultFloat32Arr, 12, 0.);
      Float32Array.unsafe_set(resultFloat32Arr, 13, 0.);
      Float32Array.unsafe_set(
        resultFloat32Arr,
        14,
        (-2.) *. far *. near *. rd,
      );
      Float32Array.unsafe_set(resultFloat32Arr, 15, 0.);

      resultFloat32Arr |> createWithoutCheck;
    },
    Debug.getIsDebug(DebugData.getDebugData()),
  );

let setTranslation = (v, mat) => {
  let resultFloat32Arr = value(mat);
  let (x, y, z) = Vector.value(v);

  Float32Array.unsafe_set(resultFloat32Arr, 12, x);
  Float32Array.unsafe_set(resultFloat32Arr, 13, y);
  Float32Array.unsafe_set(resultFloat32Arr, 14, z);

  resultFloat32Arr |> createWithoutCheck;
};