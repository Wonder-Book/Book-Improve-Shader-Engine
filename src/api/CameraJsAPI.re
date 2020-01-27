let setCameraData = ((vMatrix, pMatrix), state) =>
  Camera.setCameraData(
    (
      vMatrix |> CoordinateTransformationMatrix.View.create,
      pMatrix |> CoordinateTransformationMatrix.Projection.create,
    ),
    state,
  );