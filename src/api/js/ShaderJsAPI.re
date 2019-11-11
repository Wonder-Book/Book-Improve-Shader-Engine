/* TODO remove */
/* let addGLSL =
    (
      shaderName,
      ((vs, fs), attributeFieldNameArr, uniformFieldNameArr),
      state,
    ) =>
  Shader.GLSL.addGLSL(
    ShaderWT.Shader.create(shaderName),
    (
      (GLSLWT.VS.create(vs), GLSLWT.FS.create(fs)),
      attributeFieldNameArr
      |> Array.to_list
      |> List.map(fieldName => ShaderWT.FieldName.create(fieldName)),
      uniformFieldNameArr
      |> Array.to_list
      |> List.map(fieldName => ShaderWT.FieldName.create(fieldName)),
    ),
    state,
  ); */