let setTranslation = ((v1, v2, v3), mat) =>
  Matrix.setTranslation(Vector.create(v1, v2, v3), mat);

let buildPerspective = ((fovy, aspect, near, far), mat) =>
  Matrix.buildPerspective(
    (
      CameraWT.Frustum.Fovy.create(fovy),
      CameraWT.Frustum.Aspect.create(aspect),
      CameraWT.Frustum.Near.create(near),
      CameraWT.Frustum.Far.create(far),
    ),
    mat,
  )
  |> Result.getSuccessValue(ErrorUtils.throwError);

let setLookAt =
    ((eyeX, eyeY, eyeZ), (centerX, centerY, centerZ), (upX, upY, upZ), mat) =>
  Matrix.setLookAt(
    Vector.create(eyeX, eyeY, eyeZ),
    Vector.create(centerX, centerY, centerZ),
    Vector.create(upX, upY, upZ),
    mat,
  );

let createIdentityMatrix = Matrix.createIdentityMatrix;