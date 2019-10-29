let createTriangleGeometryData = GameObject.Geometry.createTriangleGeometryData;

let addGameObjectData =
    (mMatrix, (vertices, indices), (shaderName, colors), state) =>
  GameObject.addGameObjectData(
    CoordinateTransformationMatrix.Model.create(mMatrix),
    (vertices, indices),
    (
      ShaderWT.ShaderName.create(shaderName),
      colors
      |> Array.to_list
      |> List.map(((r, g, b)) => Color.Color3.create(r, g, b)),
    ),
    state,
  );