open StateDataType;

module Transform = {
  let createTransformData = mMatrix => {mMatrix: mMatrix};

  let getMMatrix = ({mMatrix}) => mMatrix;
};

module Geometry = {
  let createTriangleGeometryData = () => {
    open Js.Typed_array;

    let vertices =
      Float32Array.make([|
        0.,
        0.5,
        0.0,
        (-0.5),
        (-0.5),
        0.0,
        0.5,
        (-0.5),
        0.0,
      |])
      |> GeometryPoints.Vertices.create;

    let indices =
      Uint16Array.make([|0, 1, 2|]) |> GeometryPoints.Indices.create;

    (vertices, indices);
  };

  let createGeometryDataWithGeometryPoints = (vertices, indices) => {
    vertices,
    indices,
    vao: None,
  };

  let unsafeGetVAO = ({vao}: geometryData) => vao |> Option.unsafeGet;

  let setVAO = (vao: Gl.vao, geometryData: geometryData) => {
    ...geometryData,
    vao: Some(vao),
  };

  let getIndices = ({indices}) => indices;
};

module Material = {
  let createMaterialData = (shaderName, colors) => {shaderName, colors};

  let getColors = ({colors}) => colors;

  let getShaderName = ({shaderName}) => shaderName;
};

let getGameObjectDataList = state =>
  state.allGameObjectData.gameObjectDataList;

let setGameObjectDataList = (gameObjectDataList, state) => {
  ...state,
  allGameObjectData: {
    gameObjectDataList: gameObjectDataList,
  },
};

let createAllGameObjectData = () => {gameObjectDataList: []};

let addGameObjectData =
    (mMatrix, (vertices, indices), (shaderName, colors), state) =>
  setGameObjectDataList(
    [
      {
        transformData: Transform.createTransformData(mMatrix),
        geometryData:
          Geometry.createGeometryDataWithGeometryPoints(vertices, indices),
        materialData: Material.createMaterialData(shaderName, colors),
      },
      ...getGameObjectDataList(state),
    ],
    state,
  );