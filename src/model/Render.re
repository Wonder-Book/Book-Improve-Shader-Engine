open StateDataType;

open RenderDataType;

let _createVAOs =
    (
      (vertices, indices),
      shaderName,
      gl,
      vaoExt: GPUDetectType.vaoExt,
      state,
    ) => {
  let vao = vaoExt##createVertexArrayOES();

  vaoExt##bindVertexArrayOES(. Js.Nullable.return(vao));

  Shader.GLSLSender.AttributeSendData.getAllData(shaderName, state)
  |> List.iter(({buffer, sendDataFunc}: StateDataType.attributeSendData) =>
       switch (buffer) {
       | RenderConfigDataType.Vertex =>
         let vertexBuffer = Gl.createBuffer(gl);

         Gl.bindBuffer(Gl.getArrayBuffer(gl), vertexBuffer, gl);

         Gl.bufferFloat32Data(
           Gl.getArrayBuffer(gl),
           vertices,
           Gl.getStaticDraw(gl),
           gl,
         );

         sendDataFunc(vertexBuffer);
       | RenderConfigDataType.Index =>
         let indexBuffer = Gl.createBuffer(gl);

         Gl.bindBuffer(Gl.getElementArrayBuffer(gl), indexBuffer, gl);

         Gl.bufferUint16Data(
           Gl.getElementArrayBuffer(gl),
           indices,
           Gl.getStaticDraw(gl),
           gl,
         );
       }
     );

  vaoExt##bindVertexArrayOES(. Js.Nullable.null);

  vao;
};

let _initVAOs = (gl, state) =>
  GPUDetect.unsafeGetVAOExt(state)
  |> Result.map(vaoExt =>
       GameObject.getAllGameObjectData(state)
       |> List.map(
            ({transformData, geometryData, materialData} as gameObjectData) =>
            {
              ...gameObjectData,
              geometryData:
                switch (geometryData.vao) {
                | None =>
                  _createVAOs(
                    (
                      geometryData.vertices |> GeometryPoints.Vertices.value,
                      geometryData.indices |> GeometryPoints.Indices.value,
                    ),
                    GameObject.Material.getShaderName(materialData)
                    |> ShaderWT.Shader.value,
                    gl,
                    vaoExt,
                    state,
                  )
                  |> GameObject.Geometry.setVAO(_, geometryData)
                | _ => geometryData
                },
            }
          )
       |> GameObject.setAllGameObjectData(_, state)
     );

let _getProgram = (shaderName, state) =>
  Shader.Program.unsafeGetProgramByThrow(shaderName, state);

let _changeAllGameObjectDataToRenderDataList = (allGameObjectData, gl, state) =>
  GPUDetect.unsafeGetVAOExt(state)
  |> Result.bind(vaoExt =>
       allGameObjectData
       |> ListWT.traverseResultM(
            ({transformData, geometryData, materialData} as gameObjectData) =>
            GameObject.Geometry.unsafeGetVAO(geometryData)
            |> Result.map(vao => {
                 let shaderName =
                   GameObject.Material.getShaderName(materialData)
                   |> ShaderWT.Shader.value;

                 {
                   mMatrix:
                     GameObject.Transform.getMMatrix(transformData)
                     |> CoordinateTransformationMatrix.Model.getMatrixValue,
                   vao,
                   vaoExt,
                   indexCount:
                     GameObject.Geometry.getIndices(geometryData)
                     |> GeometryPoints.Indices.length,
                   colors:
                     GameObject.Material.getColors(materialData)
                     |> List.map(color => color |> Color.Color3.value),
                   program: _getProgram(shaderName, state),
                   shaderName,
                 };
               })
          )
     );

let _sendAttributeData = (vaoExt, vao, state) =>
  switch (Shader.GLSLSender.getLastSendedVAO(state)) {
  | Some(lastSendedVAO) when lastSendedVAO === vao => state
  | lastSendedVAOpt =>
    vaoExt##bindVertexArrayOES(Js.Nullable.return(vao));

    Shader.GLSLSender.setLastSendedVAO(vao, state);
  };

let _sendUniformModelData = (shaderName, mMatrix, state) =>
  Shader.GLSLSender.UniformSendData.ModelData.getAllData(shaderName, state)
  |> List.iter(
       ({sendDataFunc}: StateDataType.uniformRenderObjectSendModelData) =>
       sendDataFunc(mMatrix)
     );

let _sendUniformMaterialData = (shaderName, colors, state) =>
  Shader.GLSLSender.UniformSendData.MaterialData.getAllData(shaderName, state)
  |> List.fold_left(
       (
         state,
         {sendDataFunc}: StateDataType.uniformRenderObjectSendMaterialData,
       ) =>
         Shader.GLSLSender.setShaderCacheMap(
           shaderName,
           sendDataFunc(
             Shader.GLSLSender.unsafeGetShaderCacheMapByThrow(shaderName, state),
             colors,
           ),
           state,
         ),
       state,
     );

let _sendUniformShaderData = (gl, state) =>
  Shader.GLSL.getAllShaderNames(state)
  |> Result.bind(allShaderNames =>
       allShaderNames
       |> TinyWonderCommonlib.ArrayUtils.reduceOneParam(
            (. stateResult, shaderName) =>
              stateResult
              |> Result.map(state =>
                   Shader.GLSLSender.UniformSendData.ShaderData.getAllData(
                     shaderName,
                     state,
                   )
                 )
              |> Result.bind(allData =>
                   allData
                   |> List.fold_left(
                        (
                          stateResult,
                          {sendDataFunc}: StateDataType.uniformShaderSendData,
                        ) => {
                          let state =
                            state
                            |> Shader.Program.use(
                                 gl,
                                 Shader.Program.unsafeGetProgramByThrow(
                                   shaderName,
                                   state,
                                 ),
                               );

                          state |> sendDataFunc |> Result.map(() => state);
                        },
                        stateResult,
                      )
                 ),
            state |> Result.succeed,
          )
     );

let render = (gl, state) => {
  DeviceManager.initGlState(gl);

  state
  |> _sendUniformShaderData(gl)
  |> Result.bind(_initVAOs(gl))
  |> Result.bind(state =>
       _changeAllGameObjectDataToRenderDataList(
         GameObject.getAllGameObjectData(state),
         gl,
         state,
       )
       |> Result.bind(renderDataList =>
            renderDataList
            |> Result.tryCatch(renderDataList =>
                 renderDataList
                 |> List.fold_left(
                      (
                        state,
                        {
                          mMatrix,
                          vao,
                          vaoExt,
                          indexCount,
                          colors,
                          program,
                          shaderName,
                        },
                      ) => {
                        let state = Shader.Program.use(gl, program, state);

                        state |> _sendUniformModelData(shaderName, mMatrix);

                        let state =
                          state
                          |> _sendUniformMaterialData(shaderName, colors)
                          |> _sendAttributeData(vaoExt, vao);

                        Gl.drawElements(
                          Gl.getTriangles(gl),
                          indexCount,
                          Gl.getUnsignedShort(gl),
                          0,
                          gl,
                        );

                        state;
                      },
                      state,
                    )
               )
          )
     );
};