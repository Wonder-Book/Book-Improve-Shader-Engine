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

  let vertexBuffer = Gl.createBuffer(gl);

  Gl.bindBuffer(Gl.getArrayBuffer(gl), vertexBuffer, gl);

  Gl.bufferFloat32Data(
    Gl.getArrayBuffer(gl),
    vertices,
    Gl.getStaticDraw(gl),
    gl,
  );

  let positionLocation =
    Shader.GLSLLocation.unsafeGetAttribLocation(
      shaderName,
      "a_position",
      state,
    );
  Gl.vertexAttribPointer(
    positionLocation,
    3,
    Gl.getFloat(gl),
    false,
    0,
    0,
    gl,
  );
  Gl.enableVertexAttribArray(positionLocation, gl);

  let indexBuffer = Gl.createBuffer(gl);

  Gl.bindBuffer(Gl.getElementArrayBuffer(gl), indexBuffer, gl);

  Gl.bufferUint16Data(
    Gl.getElementArrayBuffer(gl),
    indices,
    Gl.getStaticDraw(gl),
    gl,
  );

  vaoExt##bindVertexArrayOES(. Js.Nullable.null);

  vao;
};

let _initVAOs = (gl, state) =>
  GPUDetect.unsafeGetVAOExt(state)
  |> Result.map(vaoExt =>
       GameObject.getGameObjectDataList(state)
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
                    |> ShaderWT.ShaderName.value,
                    gl,
                    vaoExt,
                    state,
                  )
                  |> GameObject.Geometry.setVAO(_, geometryData)
                | _ => geometryData
                },
            }
          )
       |> GameObject.setGameObjectDataList(_, state)
     );

let _getProgram = (shaderName, state) =>
  Shader.Program.unsafeGetProgram(shaderName, state);

let _changeGameObjectDataListToRenderDataList =
    (gameObjectDataList, gl, state) =>
  GPUDetect.unsafeGetVAOExt(state)
  |> Result.bind(vaoExt =>
       gameObjectDataList
       |> ListWT.traverseResultM(
            ({transformData, geometryData, materialData} as gameObjectData) =>
            GameObject.Geometry.unsafeGetVAO(geometryData)
            |> Result.map(vao => {
                 let shaderName =
                   GameObject.Material.getShaderName(materialData)
                   |> ShaderWT.ShaderName.value;

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

let _sendCameraUniformData =
    ((vMatrix, pMatrix), program, shaderName, gl, state) => {
  let vMatrixLocation =
    Shader.GLSLLocation.unsafeGetUniformLocation(
      shaderName,
      "u_vMatrix",
      state,
    );
  let pMatrixLocation =
    Shader.GLSLLocation.unsafeGetUniformLocation(
      shaderName,
      "u_pMatrix",
      state,
    );

  Gl.uniformMatrix4fv(vMatrixLocation, false, vMatrix, gl);
  Gl.uniformMatrix4fv(pMatrixLocation, false, pMatrix, gl);
};

let _sendModelUniformData =
    ((mMatrix, colors), program, shaderName, gl, state) => {
  let mMatrixLocation =
    Shader.GLSLLocation.unsafeGetUniformLocation(
      shaderName,
      "u_mMatrix",
      state,
    );

  Gl.uniformMatrix4fv(mMatrixLocation, false, mMatrix, gl);

  let (state, _) =
    colors
    |> List.fold_left(
         ((state, index), (r, g, b)) => {
           let colorFieldName = {j|u_color$index|j};
           let colorLocation =
             Shader.GLSLLocation.unsafeGetUniformLocation(
               shaderName,
               colorFieldName,
               state,
             );

           (
             Shader.GLSLSender.setShaderCacheMap(
               shaderName,
               Shader.GLSLSender.unsafeGetShaderCacheMap(shaderName, state)
               |> Shader.GLSLSender.sendFloat3(
                    gl,
                    (colorFieldName, colorLocation),
                    [|r, g, b|],
                  ),
               state,
             ),
             index |> succ,
           );
         },
         (state, 0),
       );

  state;
};

let _sendUniformShaderData = (gl, state) =>
  (Camera.unsafeGetVMatrix(state), Camera.unsafeGetPMatrix(state))
  |> Tuple2.sequenceResultM
  |> Result.map(((vMatrix, pMatrix)) => {
       let (vMatrix, pMatrix) = (
         vMatrix |> CoordinateTransformationMatrix.View.getMatrixValue,
         pMatrix |> CoordinateTransformationMatrix.Projection.getMatrixValue,
       );

       Shader.GLSL.getAllValidGLSLEntryList(state)
       |> List.fold_left(
            (state, (shaderName, _)) => {
              let program =
                Shader.Program.unsafeGetProgram(shaderName, state);

              let state = Shader.Program.use(gl, program, state);

              _sendCameraUniformData(
                (vMatrix, pMatrix),
                program,
                shaderName,
                gl,
                state,
              );

              state;
            },
            state,
          );
     });

let render = (gl, state) =>
  state
  |> _sendUniformShaderData(gl)
  |> Result.bind(_initVAOs(gl))
  |> Result.bind(state =>
       _changeGameObjectDataListToRenderDataList(
         GameObject.getGameObjectDataList(state),
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

                        let state =
                          state
                          |> _sendModelUniformData(
                               (mMatrix, colors),
                               program,
                               shaderName,
                               gl,
                             );

                        let state = _sendAttributeData(vaoExt, vao, state);

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