type deviceManagerData = {
  gl: option(Gl.webgl1Context),
  clearColor: Color.Color4.t,
};

type programData = {
  programMap:
    TinyWonderCommonlib.ImmutableHashMap.t2(ShaderWT.Shader.t, Gl.program),
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

type fieldName = string;

type shaderCacheMap =
  TinyWonderCommonlib.ImmutableHashMap.t2(fieldName, array(float));

type attributeSendData = {
  /* pos: (Gl.attributeLocation), */
  buffer: RenderConfigDataType.bufferEnum,
  /* size, */
  /* vao: Gl.vao, */
  sendDataFunc:
    /* (. Gl.webgl1Context, (Gl.attributeLocation, size), Gl.buffer) => */
    Gl.buffer =>
    /* SubStateSendRenderDataType.sendRenderDataSubState */
    unit,
};

type uniformRenderObjectSendModelData = {
  /* pos: Gl.uniformLocation, */
  /* getDataFunc:
       (. component, SubStateGetRenderDataType.getRenderDataSubState) =>
       Float32Array.t,
     sendDataFunc: (. webgl1Context, uniformLocation, Float32Array.t) => unit, */
  sendDataFunc: Js.Typed_array.Float32Array.t => unit,
};

type uniformRenderObjectSendMaterialData = {
  /* shaderCacheMap, */
  /* fieldName: string,
     pos: Gl.uniformLocation, */
  /* getDataFunc:
     (. component, SubStateGetRenderDataType.getRenderDataSubState) =>
     array(float), */
  sendDataFunc:
    /* . Gl.webgl1Context, */
    (
      shaderCacheMap,
      /* (string, Gl.uniformLocation), */
      array(float)
    ) =>
    /* array((float, float, float)) */
    shaderCacheMap,
};

type gpuDetectData = {vao: option(GPUDetectType.vaoExt)};

type uniformShaderSendData = {
  /* pos: Gl.uniformLocation, */
  /* getDataFunc: ( state) => Js.Typed_array.Float32Array.t, */
  sendDataFunc:
    /* (. Gl.webgl1Context, Gl.uniformLocation, Js.Typed_array.Float32Array.t) => */
    /* Js.Typed_array.Float32Array.t => unit, */
    state => Result.t(unit, Js.Exn.t),
}
and allSendUniformDataLists = {
  renderObjectSendModelDataList: list(uniformRenderObjectSendModelData),
  renderObjectSendMaterialDataList: list(uniformRenderObjectSendMaterialData),
  shaderSendDataList: list(uniformShaderSendData),
}
and glslSenderData = {
  uniformCacheMap:
    TinyWonderCommonlib.ImmutableHashMap.t2(
      ShaderWT.Shader.t,
      shaderCacheMap,
    ),
  lastBindedVAO: option(Gl.vao),
  allAttributeSendDataMap:
    TinyWonderCommonlib.ImmutableHashMap.t2(
      ShaderWT.Shader.t,
      list(attributeSendData),
    ),
  allUniformRenderObjectSendModelDataMap:
    TinyWonderCommonlib.ImmutableHashMap.t2(
      ShaderWT.Shader.t,
      list(uniformRenderObjectSendModelData),
    ),
  allUniformRenderObjectSendMaterialDataMap:
    TinyWonderCommonlib.ImmutableHashMap.t2(
      ShaderWT.Shader.t,
      list(uniformRenderObjectSendMaterialData),
    ),
  allUniformShaderSendDataMap:
    TinyWonderCommonlib.ImmutableHashMap.t2(
      ShaderWT.Shader.t,
      list(uniformShaderSendData),
    ),
}
and state = {
  viewData,
  deviceManagerData,
  gpuDetectData,
  glslSenderData,
  programData,
  cameraData,
  allGameObjectData,
  glslChunkData: ShaderChunkType.glslChunkData,
  renderConfigData: option(RenderConfigDataType.renderConfigData),
};

type stateData = {mutable state: option(state)};

external floatArrToFloatTuple3: array(float) => (float, float, float) =
  "%identity";

external floatTuple3ToFloatArr: ((float, float, float)) => array(float) =
  "%identity";