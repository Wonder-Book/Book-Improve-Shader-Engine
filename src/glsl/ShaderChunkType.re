type glslChunk = {
  top: string,
  define: string,
  varDeclare: string,
  funcDeclare: string,
  funcDefine: string,
  body: string,
};

type glslChunkData = {
  chunkMap: TinyWonderCommonlib.ImmutableHashMap.t2(ShaderWT.Shader.t, glslChunk),
};