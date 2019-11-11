type deviceManagerData = {
  gl: option(Gl.webgl1Context),
  clearColor: Color.Color4.t,
};

/* type glslData = {
  glslMap:
    TinyWonderCommonlib.ImmutableHashMap.t2(
      ShaderWT.Shader.t,
      (
        (GLSLWT.VS.t, GLSLWT.FS.t),
        list(ShaderWT.FieldName.t),
        list(ShaderWT.FieldName.t),
      ),
    ),
}; */

type programData = {
  programMap:
    TinyWonderCommonlib.ImmutableHashMap.t2(
      ShaderWT.Shader.t,
      Gl.program,
    ),
  lastUsedProgram: option(Gl.program),
};

type geometryData = {
  vertices: GeometryPoints.Vertices.t,
  indices: GeometryPoints.Indices.t,
  vao: option(Gl.vao),
};

type transformData = {mMatrix: CoordinateTransformationMatrix.Model.t};

type materialData = {
  shaderName: ShaderWT.Shader.t,
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
  TinyWonderCommonlib.ImmutableHashMap.t2(
    ShaderWT.Shader.t,
    TinyWonderCommonlib.ImmutableHashMap.t2(
      ShaderWT.FieldName.t,
      Gl.attributeLocation,
    ),
  );

type uniformLocationMap =
  TinyWonderCommonlib.ImmutableHashMap.t2(
    ShaderWT.Shader.t,
    TinyWonderCommonlib.ImmutableHashMap.t2(
      ShaderWT.FieldName.t,
      Gl.uniformLocation,
    ),
  );

type glslLocationData = {
  attributeLocationMap,
  uniformLocationMap,
};

type shaderCacheMap =
  TinyWonderCommonlib.ImmutableHashMap.t2(
    ShaderWT.FieldName.t,
    array(float),
  );

type glslSenderData = {
  uniformCacheMap:
    TinyWonderCommonlib.ImmutableHashMap.t2(
      ShaderWT.Shader.t,
      shaderCacheMap,
    ),
  lastBindedVAO: option(Gl.vao),
};

type gpuDetectData = {vao: option(GPUDetectType.vaoExt)};

type state = {
  viewData,
  deviceManagerData,
  gpuDetectData,
  /* glslData, */
  glslLocationData,
  glslSenderData,
  programData,
  cameraData,
  allGameObjectData,
  glslChunkData: ShaderChunkType.glslChunkData,
  renderConfigData: option(RenderConfigDataType.renderConfigData),
};

type stateData = {mutable state: option(state)};