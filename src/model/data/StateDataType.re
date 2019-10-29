type deviceManagerData = {
  gl: option(Gl.webgl1Context),
  clearColor: Color.Color4.t,
};

type glslData = {
  glslMap:
    ImmutableHashMap.t2(
      ShaderWT.ShaderName.t,
      (
        (GLSLWT.VS.t, GLSLWT.FS.t),
        list(ShaderWT.FieldName.t),
        list(ShaderWT.FieldName.t),
      ),
    ),
};

type programData = {
  programMap: ImmutableHashMap.t2(ShaderWT.ShaderName.t, Gl.program),
  lastUsedProgram: option(Gl.program),
};

type geometryData = {
  vertices: GeometryPoints.Vertices.t,
  indices: GeometryPoints.Indices.t,
  vao: option(Gl.vao),
};

type transformData = {mMatrix: CoordinateTransformationMatrix.Model.t};

type materialData = {
  shaderName: ShaderWT.ShaderName.t,
  colors: list(Color.Color3.t),
};

type gameObjectData = {
  transformData,
  geometryData,
  materialData,
};

type allGameObjectData = {gameObjectDataList: list(gameObjectData)};

type cameraData = {
  vMatrix: option(CoordinateTransformationMatrix.View.t),
  pMatrix: option(CoordinateTransformationMatrix.Projection.t),
};

type canvas = DomExtend.htmlElement;

type viewData = {canvas: option(canvas)};

type attributeLocationMap =
  ImmutableHashMap.t2(
    ShaderWT.ShaderName.t,
    ImmutableHashMap.t2(ShaderWT.FieldName.t, Gl.attributeLocation),
  );

type uniformLocationMap =
  ImmutableHashMap.t2(
    ShaderWT.ShaderName.t,
    ImmutableHashMap.t2(ShaderWT.FieldName.t, Gl.uniformLocation),
  );

type glslLocationData = {
  attributeLocationMap,
  uniformLocationMap,
};

type shaderCacheMap =
  ImmutableHashMap.t2(ShaderWT.FieldName.t, array(float));

type glslSenderData = {
  uniformCacheMap: ImmutableHashMap.t2(ShaderWT.ShaderName.t, shaderCacheMap),
  lastBindedVAO: option(Gl.vao),
};

type gpuDetectData = {vao: option(GPUDetectType.vaoExt)};

type state = {
  viewData,
  deviceManagerData,
  gpuDetectData,
  glslData,
  glslLocationData,
  glslSenderData,
  programData,
  cameraData,
  allGameObjectData,
};

type stateData = {mutable state: option(state)};