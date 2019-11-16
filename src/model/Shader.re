open StateDataType;

module GLSL = {
  let createGLSLData = () => {precision: None};

  let _getPrecisionSource = ({gpuDetectData, glslChunkData}) => {
    open ShaderChunkSystem;
    open GPUDetectType;

    let {precision}: StateDataType.gpuDetectData = gpuDetectData;
    let default = getChunk("highp", glslChunkData).top;
    switch (gpuDetectData.precision) {
    | None => default
    | Some(precision) =>
      switch (precision) {
      | HIGHP => getChunk("highp", glslChunkData).top
      | MEDIUMP => getChunk("mediump", glslChunkData).top
      | LOWP => getChunk("lowp", glslChunkData).top
      | _ => default
      }
    };
  };

  let setPrecision = state => {
    ...state,
    glslData: {
      precision: Some(_getPrecisionSource(state)),
    },
  };

  let getWebgl1MainBegin = () => "void main(void){\n";

  let getWebgl1MainEnd = () => "}\n";

  let _createEmptyChunk = (): ShaderChunkType.glslChunk => {
    top: "",
    define: "",
    varDeclare: "",
    funcDeclare: "",
    funcDefine: "",
    body: "",
  };

  let _setSource =
      (
        {
          top: sourceTop,
          define: sourceDefine,
          varDeclare: sourceVarDeclare,
          funcDeclare: sourceFuncDeclare,
          funcDefine: sourceFuncDefine,
          body: sourceBody,
        }: ShaderChunkType.glslChunk,
        {top, define, varDeclare, funcDeclare, funcDefine, body}: ShaderChunkType.glslChunk,
      )
      : ShaderChunkType.glslChunk => {
    top: sourceTop ++ top,
    define: sourceDefine ++ define,
    varDeclare: sourceVarDeclare ++ varDeclare,
    funcDeclare: sourceFuncDeclare ++ funcDeclare,
    funcDefine: sourceFuncDefine ++ funcDefine,
    body: sourceBody ++ body,
  };

  let _buildVsAndFsByType =
      /* ((vs, fs), (type_, name), execHandleFunc, glslChunkData) => */
      ((vs, fs), (type_, name), glslChunkData) =>
    switch (type_) {
    | "vs" => (
        _setSource(vs, ShaderChunkSystem.getChunk(name, glslChunkData)),
        fs,
      )
    | "fs" => (
        vs,
        _setSource(fs, ShaderChunkSystem.getChunk(name, glslChunkData)),
      )
    | type_ =>
      ErrorUtils.raiseErrorAndReturn(
        LogUtils.buildFatalMessage(
          ~title="_buildVsAndFsByType",
          ~description={j|unknown glsl type: $type_|j},
          ~reason="",
          ~solution={j||j},
          ~params={j|name: $name|j},
        ),
      )
    };

  let _buildVsAndFs = (glslTuple, shaderLibs, glslChunkData) =>
    shaderLibs
    /* |> ArrayWT.traverseResultM( */
    |> TinyWonderCommonlib.ArrayUtils.reduceOneParam(
         (. glslTuple, {glsls}: RenderConfigDataType.shaderLib) =>
           /* OptionService.isJsonSerializedValueNone(glsls) ?
              glslTuple :
              glsls
              |> OptionService.unsafeGetJsonSerializedValue
              |> WonderCommonlib.ArrayService.reduceOneParam(
                   (. sourceTuple, {type_, name}: glsl) =>
                     _buildVsAndFsByType(
                       sourceTuple,
                       (type_, name),
                       /* execHandleFunc, */
                       glslChunkData,
                     ),
                   glslTuple,
                 ), */
           switch (glsls) {
           | None => glslTuple
           | Some(glsls) =>
             glsls
             /* |> OptionService.unsafeGetJsonSerializedValue */
             |> TinyWonderCommonlib.ArrayUtils.reduceOneParam(
                  (. glslTuple, {type_, name}: RenderConfigDataType.glsl) =>
                    _buildVsAndFsByType(
                      glslTuple,
                      (type_, name),
                      /* execHandleFunc, */
                      glslChunkData,
                    ),
                  glslTuple,
                )
           },
         glslTuple,
       );

  let _buildBody = ({body}: ShaderChunkType.glslChunk, webgl1_main_end) =>
    body ++ webgl1_main_end;

  let _generateAttributeSource = shaderLibs =>
    shaderLibs
    |> TinyWonderCommonlib.ArrayUtils.reduceOneParam(
         (. result: string, {variables}: RenderConfigDataType.shaderLib) =>
           switch (variables) {
           | None => result
           | Some(variables) =>
             switch (variables.attributes) {
             | None => result
             | Some(attributes) =>
               result
               ++ (
                 attributes
                 |> TinyWonderCommonlib.ArrayUtils.reduceOneParam(
                      (.
                        result: string,
                        {name, type_}: RenderConfigDataType.attribute,
                      ) =>
                        switch (name, type_) {
                        | (Some(name), Some(type_)) =>
                          result ++ {j|attribute $type_ $name;
  |j}
                        | _ => result
                        },
                      "",
                    )
               )
             }
           },
         "",
       );

  let _isInSource = (key: string, source: string) =>
    Js.String.indexOf(key, source) > (-1);

  let _generateUniformSourceType = (type_: string) =>
    switch (type_) {
    | "float3" => "vec3"
    | _ => type_
    };

  let _generateUniformSource =
      (
        shaderLibs,
        sourceVarDeclare: string,
        sourceFuncDefine: string,
        sourceBody: string,
      ) =>
    shaderLibs
    |> TinyWonderCommonlib.ArrayUtils.reduceOneParam(
         (. result: string, {variables}: RenderConfigDataType.shaderLib) =>
           switch (variables) {
           | None => result
           | Some(variables) =>
             switch (variables.uniforms) {
             | None => result
             | Some(uniforms) =>
               result
               ++ (
                 uniforms
                 |> Js.Array.filter(({name}: RenderConfigDataType.uniform) =>
                      _isInSource(name, sourceVarDeclare)
                      || _isInSource(name, sourceFuncDefine)
                      || _isInSource(name, sourceBody)
                    )
                 |> TinyWonderCommonlib.ArrayUtils.reduceOneParam(
                      (.
                        result: string,
                        {name, type_}: RenderConfigDataType.uniform,
                      ) => {
                        let type_ = _generateUniformSourceType(type_);
                        result ++ {j|uniform $type_ $name;
|j};
                      },
                      "",
                    )
               )
             }
           },
         "",
       );

  let _buildVarDeclare =
      (
        {top, varDeclare, funcDefine, body}: ShaderChunkType.glslChunk,
        shaderLibs,
      ) =>
    /* "\n"
       ++ _generateAttributeSource(shaderLibs) */
    varDeclare
    ++ "\n"
    ++ _generateUniformSource(shaderLibs, varDeclare, funcDefine, body);

  let _addAlllParts =
      (
        {top, define, varDeclare, funcDeclare, funcDefine, body}: ShaderChunkType.glslChunk,
      ) =>
    top ++ define ++ varDeclare ++ funcDeclare ++ funcDefine ++ body;

  let buildGLSLSource =
      /* shaderLibs, */
      (
        shaderLibs,
        /* execHandleFunc, */
        /* (glslRecord, glslChunkData), */
        {glslData, glslChunkData},
      ) => {
    open ShaderChunkType;

    let vs: glslChunk = _createEmptyChunk();
    let fs: glslChunk = _createEmptyChunk();
    let {precision}: StateDataType.glslData = glslData;
    let precision = precision |> Option.unsafeGetByThrow;
    let vs = {
      ...vs,
      top: precision ++ vs.top,
      body: vs.body ++ getWebgl1MainBegin(),
    };
    let fs = {
      ...fs,
      top: precision ++ fs.top,
      body: fs.body ++ getWebgl1MainBegin(),
    };
    let (vs, fs) =
      _buildVsAndFs(
        (vs, fs),
        shaderLibs,
        /* execHandleFunc, */
        glslChunkData,
      );

    (
      {
        ...vs,
        body: _buildBody(vs, getWebgl1MainEnd()),
        varDeclare:
          "\n"
          ++ _generateAttributeSource(shaderLibs)
          ++ vs.varDeclare
          ++ _buildVarDeclare(vs, shaderLibs),
      }
      |> _addAlllParts,
      {
        ...fs,
        body: _buildBody(fs, getWebgl1MainEnd()),
        varDeclare: _buildVarDeclare(fs, shaderLibs),
      }
      |> _addAlllParts,
    );
  };

  let getAllGLSLData = state =>
    Tuple3.sequenceResultM((
      Config.Render.getGroups(state),
      Config.Render.getAllShaders(state),
      Config.Render.getAllShaderLibs(state),
    ));

  let getAllShaderNames = state =>
    Config.Render.getAllShaders(state)
    |> Result.map(shaders =>
         shaders
         |> Js.Array.map(({name}: RenderConfigDataType.shader) => name)
       );
};

module Program = {
  let createProgramData = () => {
    programMap: TinyWonderCommonlib.ImmutableHashMap.createEmpty(),
    lastUsedProgram: None,
  };

  let createProgram = gl => gl |> Gl.createProgram;

  let _getProgramMap = state => state.programData.programMap;

  let _setProgramMap = (programMap, state) => {
    ...state,
    programData: {
      ...state.programData,
      programMap,
    },
  };

  let unsafeGetProgram = (shaderName, state) => {
    ContractUtils.requireCheckByThrow(
      () =>
        ContractUtils.(
          Operators.(
            test(
              LogUtils.buildAssertMessage(
                ~expect={j|program of shader:$shaderName exist|j},
                ~actual={j|not|j},
              ),
              () =>
              _getProgramMap(state)
              |> TinyWonderCommonlib.ImmutableHashMap.has(shaderName)
              |> assertTrue
            )
          )
        ),
      Debug.getIsDebug(DebugData.getDebugData()),
    );

    _getProgramMap(state)
    |> TinyWonderCommonlib.ImmutableHashMap.unsafeGet(shaderName);
  };

  let setProgram = (shaderName, program, state) =>
    _setProgramMap(
      _getProgramMap(state)
      |> TinyWonderCommonlib.ImmutableHashMap.set(shaderName, program),
      state,
    );

  let use = (gl, program, {programData} as state) =>
    switch (programData.lastUsedProgram) {
    | Some(lastUsedProgram) when program === lastUsedProgram => state
    | _ =>
      Gl.useProgram(program, gl);

      {
        ...state,
        programData: {
          ...state.programData,
          lastUsedProgram: Some(program),
        },
      };
    };
};

module GLSLLocation = {
  let isAttribLocationExist = pos => pos !== (-1);

  let getAttribLocation = (program, fieldName, gl) =>
    Gl.getAttribLocation(program, fieldName, gl);

  let isUniformLocationExist = pos => !Js.Null.test(pos);

  let getUniformLocation = (program, fieldName, gl) => {
    let location = Gl.getUniformLocation(program, fieldName, gl);

    Obj.magic(location)##fieldName #= fieldName;

    location;
  };
};

module GLSLSender = {
  let createGLSLSenderData = () => {
    uniformCacheMap: TinyWonderCommonlib.ImmutableHashMap.createEmpty(),
    lastBindedVAO: None,
    allAttributeSendDataMap:
      TinyWonderCommonlib.ImmutableHashMap.createEmpty(),
    allUniformRenderObjectSendModelDataMap:
      TinyWonderCommonlib.ImmutableHashMap.createEmpty(),
    allUniformRenderObjectSendMaterialDataMap:
      TinyWonderCommonlib.ImmutableHashMap.createEmpty(),
    allUniformShaderSendDataMap:
      TinyWonderCommonlib.ImmutableHashMap.createEmpty(),
  };

  let _fastGetCache = (shaderCacheMap, name: string) =>
    shaderCacheMap |> TinyWonderCommonlib.ImmutableHashMap.fastGet(name);

  let _queryIsNotCacheWithCache = (cache, x, y, z) => {
    let isNotCached = ref(false);
    if (Array.unsafe_get(cache, 0) !== x) {
      Array.unsafe_set(cache, 0, x);
      isNotCached := true;
    };
    if (Array.unsafe_get(cache, 1) !== y) {
      Array.unsafe_set(cache, 1, y);
      isNotCached := true;
    };
    if (Array.unsafe_get(cache, 2) !== z) {
      Array.unsafe_set(cache, 2, z);
      isNotCached := true;
    };
    isNotCached^;
  };

  let _setCache = (shaderCacheMap, name: string, record) =>
    shaderCacheMap |> TinyWonderCommonlib.ImmutableHashMap.set(name, record);

  let _isNotCacheVector3AndSetCache =
      (shaderCacheMap, name: string, (x: float, y: float, z: float)) => {
    let (has, cache) = _fastGetCache(shaderCacheMap, name);

    has ?
      (shaderCacheMap, _queryIsNotCacheWithCache(cache, x, y, z)) :
      (_setCache(shaderCacheMap, name, [|x, y, z|]), true);
  };

  let sendFloat3 =
      (
        gl,
        (name: string, pos: Gl.uniformLocation),
        (x, y, z),
        shaderCacheMap,
      ) => {
    let (shaderCacheMap, isNotCache) =
      _isNotCacheVector3AndSetCache(shaderCacheMap, name, (x, y, z));

    if (isNotCache) {
      Gl.uniform3f(pos, x, y, z, gl);
    } else {
      ();
    };

    shaderCacheMap;
  };

  let getUniformCacheMap = state => state.glslSenderData.uniformCacheMap;

  let setUniformCacheMap = (uniformCacheMap, state) => {
    ...state,
    glslSenderData: {
      ...state.glslSenderData,
      uniformCacheMap,
    },
  };

  let unsafeGetShaderCacheMap = (shaderName, state) => {
    ContractUtils.requireCheckByThrow(
      () =>
        ContractUtils.(
          Operators.(
            test(
              LogUtils.buildAssertMessage(
                ~expect={j|shaderCacheMap of shader:$shaderName exist|j},
                ~actual={j|not|j},
              ),
              () =>
              getUniformCacheMap(state)
              |> TinyWonderCommonlib.ImmutableHashMap.has(shaderName)
              |> assertTrue
            )
          )
        ),
      Debug.getIsDebug(DebugData.getDebugData()),
    );

    getUniformCacheMap(state)
    |> TinyWonderCommonlib.ImmutableHashMap.unsafeGet(shaderName);
  };

  let setShaderCacheMap = (shaderName, shaderCacheMap, state) =>
    getUniformCacheMap(state)
    |> TinyWonderCommonlib.ImmutableHashMap.set(shaderName, shaderCacheMap)
    |> setUniformCacheMap(_, state);

  let createShaderCacheMap = (shaderName, state) =>
    getUniformCacheMap(state)
    |> TinyWonderCommonlib.ImmutableHashMap.set(
         shaderName,
         TinyWonderCommonlib.ImmutableHashMap.createEmpty(),
       )
    |> setUniformCacheMap(_, state);

  let getLastSendedVAO = state => state.glslSenderData.lastBindedVAO;

  let setLastSendedVAO = (lastBindedVAO, state) => {
    ...state,
    glslSenderData: {
      ...state.glslSenderData,
      lastBindedVAO: Some(lastBindedVAO),
    },
  };

  module AttributeSendData = {
    let getAllDataMap = state => state.glslSenderData.allAttributeSendDataMap;

    let setAllDataMap = (map, state) => {
      ...state,
      glslSenderData: {
        ...state.glslSenderData,
        allAttributeSendDataMap: map,
      },
    };

    let getAllData = (shaderName, state) =>
      switch (
        getAllDataMap(state)
        |> TinyWonderCommonlib.ImmutableHashMap.get(shaderName)
      ) {
      | None => []
      | Some(allData) => allData
      };

    let addAllData = ((shaderName, shaderLibs), gl, program, state) =>
      getAllDataMap(state)
      |> TinyWonderCommonlib.ImmutableHashMap.set(
           shaderName,
           shaderLibs
           |> TinyWonderCommonlib.ArrayUtils.reduceOneParam(
                (. sendDataList, {variables}: RenderConfigDataType.shaderLib) =>
                  switch (variables) {
                  | None => sendDataList
                  | Some({attributes}) =>
                    switch (attributes) {
                    | None => sendDataList
                    | Some(attributes) =>
                      attributes
                      |> TinyWonderCommonlib.ArrayUtils.reduceOneParam(
                           (.
                             sendDataList,
                             {name, buffer}: RenderConfigDataType.attribute,
                           ) =>
                             switch (buffer) {
                             | RenderConfigDataType.Vertex =>
                               let name = Option.unsafeGetByThrow(name);

                               switch (
                                 GLSLLocation.getAttribLocation(
                                   program,
                                   name,
                                   gl,
                                 )
                               ) {
                               | pos
                                   when
                                     GLSLLocation.isAttribLocationExist(pos) => [
                                   {
                                     buffer,
                                     sendDataFunc: buffer => {
                                       Gl.bindBuffer(
                                         Gl.getArrayBuffer(gl),
                                         buffer,
                                         gl,
                                       );
                                       Gl.vertexAttribPointer(
                                         pos,
                                         3,
                                         Gl.getFloat(gl),
                                         false,
                                         0,
                                         0,
                                         gl,
                                       );
                                       Gl.enableVertexAttribArray(pos, gl);
                                     },
                                   },
                                   ...sendDataList,
                                 ]
                               | _ => sendDataList
                               };
                             | RenderConfigDataType.Index => [
                                 {buffer, sendDataFunc: _buffer => ()},
                                 ...sendDataList,
                               ]
                             },
                           sendDataList,
                         )
                    }
                  },
                [],
              ),
         )
      |> setAllDataMap(_, state);
  };

  module UniformSendData = {
    module ModelData = {
      let getAllDataMap = state =>
        state.glslSenderData.allUniformRenderObjectSendModelDataMap;

      let setAllDataMap = (map, state) => {
        ...state,
        glslSenderData: {
          ...state.glslSenderData,
          allUniformRenderObjectSendModelDataMap: map,
        },
      };

      let getAllData = (shaderName, state) =>
        switch (
          getAllDataMap(state)
          |> TinyWonderCommonlib.ImmutableHashMap.get(shaderName)
        ) {
        | None => []
        | Some(allData) => allData
        };

      let _isModelData = field => field === "mMatrix";

      let addAllData = ((name, field), gl, program, sendDataList) =>
        _isModelData(field) ?
          switch (GLSLLocation.getUniformLocation(program, name, gl)) {
          | pos when GLSLLocation.isUniformLocationExist(pos) =>
            let pos = Js.Null.getUnsafe(pos);

            switch (field) {
            | "mMatrix" => [
                (
                  {
                    sendDataFunc: matrixValue =>
                      Gl.uniformMatrix4fv(pos, false, matrixValue, gl),
                  }: StateDataType.uniformRenderObjectSendModelData
                ),
                ...sendDataList,
              ]
            };
          | _ => sendDataList
          } :
          sendDataList;
    };

    module MaterialData = {
      let getAllDataMap = state =>
        state.glslSenderData.allUniformRenderObjectSendMaterialDataMap;

      let setAllDataMap = (map, state) => {
        ...state,
        glslSenderData: {
          ...state.glslSenderData,
          allUniformRenderObjectSendMaterialDataMap: map,
        },
      };

      let getAllData = (shaderName, state) =>
        switch (
          getAllDataMap(state)
          |> TinyWonderCommonlib.ImmutableHashMap.get(shaderName)
        ) {
        | None => []
        | Some(allData) => allData
        };

      let _isMaterialData = field => field === "color0" || field === "color1";

      let addAllData = ((name, field), gl, program, sendDataList) =>
        _isMaterialData(field) ?
          switch (GLSLLocation.getUniformLocation(program, name, gl)) {
          | pos when GLSLLocation.isUniformLocationExist(pos) =>
            let pos = Js.Null.getUnsafe(pos);

            switch (field) {
            | "color0" => [
                (
                  {
                    sendDataFunc: (shaderCacheMap, colors) =>
                      shaderCacheMap
                      |> sendFloat3(gl, (field, pos), colors |> List.hd),
                  }: StateDataType.uniformRenderObjectSendMaterialData
                ),
                ...sendDataList,
              ]
            | "color1" => [
                (
                  {
                    sendDataFunc: (shaderCacheMap, colors) =>
                      shaderCacheMap
                      |> sendFloat3(gl, (field, pos), colors->List.nth(1)),
                  }: StateDataType.uniformRenderObjectSendMaterialData
                ),
                ...sendDataList,
              ]
            };
          | _ => sendDataList
          } :
          sendDataList;
    };

    module ShaderData = {
      let getAllDataMap = state =>
        state.glslSenderData.allUniformShaderSendDataMap;

      let setAllDataMap = (map, state) => {
        ...state,
        glslSenderData: {
          ...state.glslSenderData,
          allUniformShaderSendDataMap: map,
        },
      };

      let getAllData = (shaderName, state) =>
        switch (
          getAllDataMap(state)
          |> TinyWonderCommonlib.ImmutableHashMap.get(shaderName)
        ) {
        | None => []
        | Some(allData) => allData
        };

      let _isShaderData = field => field === "vMatrix" || field === "pMatrix";

      let addAllData = ((name, field), gl, program, sendDataList) =>
        _isShaderData(field) ?
          switch (GLSLLocation.getUniformLocation(program, name, gl)) {
          | pos when GLSLLocation.isUniformLocationExist(pos) =>
            let pos = Js.Null.getUnsafe(pos);

            switch (field) {
            | "vMatrix" => [
                (
                  {
                    sendDataFunc: state =>
                      Camera.unsafeGetVMatrix(state)
                      |> Result.map(vMatrix =>
                           Gl.uniformMatrix4fv(
                             pos,
                             false,
                             vMatrix
                             |> CoordinateTransformationMatrix.View.getMatrixValue,
                             gl,
                           )
                         ),
                  }: StateDataType.uniformShaderSendData
                ),
                ...sendDataList,
              ]
            | "pMatrix" => [
                (
                  {
                    sendDataFunc: state =>
                      Camera.unsafeGetPMatrix(state)
                      |> Result.map(pMatrix =>
                           Gl.uniformMatrix4fv(
                             pos,
                             false,
                             pMatrix
                             |> CoordinateTransformationMatrix.Projection.getMatrixValue,
                             gl,
                           )
                         ),
                  }: StateDataType.uniformShaderSendData
                ),
                ...sendDataList,
              ]
            };
          | _ => sendDataList
          } :
          sendDataList;
    };

    let addAllData = ((shaderName, shaderLibs), gl, program, state) => {
      let {
        renderObjectSendModelDataList,
        renderObjectSendMaterialDataList,
        shaderSendDataList,
      }: StateDataType.allSendUniformDataLists =
        shaderLibs
        |> TinyWonderCommonlib.ArrayUtils.reduceOneParam(
             (.
               allSendUniformDataLists: StateDataType.allSendUniformDataLists,
               {variables}: RenderConfigDataType.shaderLib,
             ) =>
               switch (variables) {
               | None => allSendUniformDataLists
               | Some({uniforms}) =>
                 switch (uniforms) {
                 | None => allSendUniformDataLists
                 | Some(uniforms) =>
                   uniforms
                   |> TinyWonderCommonlib.ArrayUtils.reduceOneParam(
                        (.
                          {
                            renderObjectSendModelDataList,
                            renderObjectSendMaterialDataList,
                            shaderSendDataList,
                          },
                          {name, field, type_}: RenderConfigDataType.uniform,
                        ) => {
                          renderObjectSendModelDataList:
                            ModelData.addAllData(
                              (name, field),
                              gl,
                              program,
                              renderObjectSendModelDataList,
                            ),
                          renderObjectSendMaterialDataList:
                            MaterialData.addAllData(
                              (name, field),
                              gl,
                              program,
                              renderObjectSendMaterialDataList,
                            ),
                          shaderSendDataList:
                            ShaderData.addAllData(
                              (name, field),
                              gl,
                              program,
                              shaderSendDataList,
                            ),
                        },
                        allSendUniformDataLists,
                      )
                 }
               },
             {
               renderObjectSendModelDataList: [],
               renderObjectSendMaterialDataList: [],
               shaderSendDataList: [],
             }: StateDataType.allSendUniformDataLists,
           );

      let state =
        ModelData.getAllDataMap(state)
        |> TinyWonderCommonlib.ImmutableHashMap.set(
             shaderName,
             renderObjectSendModelDataList,
           )
        |> ModelData.setAllDataMap(_, state);

      let state =
        MaterialData.getAllDataMap(state)
        |> TinyWonderCommonlib.ImmutableHashMap.set(
             shaderName,
             renderObjectSendMaterialDataList,
           )
        |> MaterialData.setAllDataMap(_, state);

      let state =
        ShaderData.getAllDataMap(state)
        |> TinyWonderCommonlib.ImmutableHashMap.set(
             shaderName,
             shaderSendDataList,
           )
        |> ShaderData.setAllDataMap(_, state);

      state;
    };
  };
};

let _compileShader = (gl, glslSource: string, shader) => {
  Gl.shaderSource(shader, glslSource, gl);
  Gl.compileShader(shader, gl);

  shader
  |> ContractUtils.ensureCheckByThrow(
       shader =>
         ContractUtils.(
           Gl.getShaderParameter(shader, Gl.getCompileStatus(gl), gl)
           === false ?
             {
               let message = Gl.getShaderInfoLog(shader, gl);

               LogUtils.log({j|shader info log:
          $message|j});
               LogUtils.log({j|glsl source:
          $glslSource|j});

               assertFail();
             } :
             ()
         ),
       Debug.getIsDebug(DebugData.getDebugData()),
     );
};

let _linkProgram = (program, gl) => {
  Gl.linkProgram(program, gl);

  ()
  |> ContractUtils.ensureCheckByThrow(
       () =>
         ContractUtils.
           /*! perf: slow

             let message = Gl.getProgramInfoLog(program, gl);

             test(
               LogUtils.buildAssertMessage(
                 ~expect={j|link program success|j},
                 ~actual={j|fail: $message|j},
               ),
               () =>
               Gl.getProgramParameter(program, Gl.getLinkStatus(gl), gl)
               |> assertFalse
             ); */
           /*! perf: faster */
           (
             Gl.getProgramParameter(program, Gl.getLinkStatus(gl), gl)
             === false ?
               {
                 let message = Gl.getProgramInfoLog(program, gl);

                 LogUtils.buildAssertMessage(
                   ~expect={j|link program success|j},
                   ~actual={j|fail: $message|j},
                 )
                 |> assertFailWithMessage;
               } :
               ()
           ),
       Debug.getIsDebug(DebugData.getDebugData()),
     );
};

let _initShader = (vsSource: string, fsSource: string, gl, program) => {
  let vs =
    _compileShader(
      gl,
      vsSource,
      Gl.createShader(Gl.getVertexShader(gl), gl),
    );
  let fs =
    _compileShader(
      gl,
      fsSource,
      Gl.createShader(Gl.getFragmentShader(gl), gl),
    );

  Gl.attachShader(program, vs, gl);
  Gl.attachShader(program, fs, gl);

  /*!
    if warn:"Attribute 0 is disabled. This has significant performance penalty" when run,
    then do this before linkProgram:
    gl.bindAttribLocation( this.glProgram, 0, "a_position");



    can reference here:
    http://stackoverflow.com/questions/20305231/webgl-warning-attribute-0-is-disabled-this-has-significant-performance-penalt?answertab=votes#tab-top


    OpenGL requires attribute zero to be enabled otherwise it will not render anything.
    On the other hand OpenGL ES 2.0 on which WebGL is based does not. So, to emulate OpenGL ES 2.0 on top of OpenGL if you don't enable attribute 0 the browser has to make a buffer for you large enough for the number of vertices you've requested to be drawn, fill it with the correct value (see gl.vertexAttrib),
    attach it to attribute zero, and enable it.

    It does all this behind the scenes but it's important for you to know that it takes time to create and fill that buffer. There are optimizations the browser can make but in the general case,
    if you were to assume you were running on OpenGL ES 2.0 and used attribute zero as a constant like you are supposed to be able to do, without the warning you'd have no idea of the work the browser is doing on your behalf to emulate that feature of OpenGL ES 2.0 that is different from OpenGL.

    require your particular case the warning doesn't have much meaning. It looks like you are only drawing a single point. But it would not be easy for the browser to figure that out so it just warns you anytime you draw and attribute 0 is not enabled.
    */
  /*!
    Always have vertex attrib 0 array enabled. If you draw with vertex attrib 0 array disabled, you will force the browser to do complicated emulation when running on desktop OpenGL (e.g. on Mac OSX). This is because in desktop OpenGL, nothing gets drawn if vertex attrib 0 is not array-enabled. You can use bindAttribLocation() to force a vertex attribute to use location 0, and use enableVertexAttribArray() to make it array-enabled.
    */
  Gl.bindAttribLocation(program, 0, "a_position", gl);

  _linkProgram(program, gl);

  /*!
    should detach and delete shaders after linking the program

    explain:
    The shader object, due to being attached to the program object, will continue to exist even if you delete the shader object. It will only be deleted by the system when it is no longer attached to any program object (and when the user has asked to delete it, of course).

    "Deleting" the shader, as with all OpenGL objects, merely sets a flag that says you don't need it any more. OpenGL will keep it around for as long as it needs it itself, and will do the actual delete any time later (most likely, but not necessarily, after the program is deleted).
    */
  Gl.deleteShader(vs, gl);
  Gl.deleteShader(fs, gl);

  program;
};

let _changeAllGLSLDataToInitShaderDataArr =
    allGLSLData: Result.t(array(InitShaderDataType.initShaderData), Js.Exn.t) =>
  allGLSLData
  |> Result.map(((groups, shaders, shaderLibs)) =>
       shaders
       |> TinyWonderCommonlib.ArrayUtils.reduceOneParam(
            (.
              initShaderDataArr,
              {name, shaderLibItems}: RenderConfigDataType.shader,
            ) =>
              initShaderDataArr
              |> ArrayWT.push(
                   {
                     shaderName: name,
                     shaderLibs:
                       Config.Render.getShaderLibDataArr(
                         groups,
                         shaderLibItems,
                         shaderLibs,
                       ),
                   }: InitShaderDataType.initShaderData,
                 ),
            ArrayWT.createEmpty(),
          )
     );

let init =
    (state: StateDataType.state): Result.t(StateDataType.state, Js.Exn.t) =>
  DeviceManager.unsafeGetGl(state)
  |> Result.bind(gl =>
       GLSL.getAllGLSLData(state)
       |> _changeAllGLSLDataToInitShaderDataArr
       |> Result.bind(initShaderDataArr =>
            initShaderDataArr
            |> Result.tryCatch(initShaderDataArr =>
                 initShaderDataArr
                 |> TinyWonderCommonlib.ArrayUtils.reduceOneParam(
                      (.
                        state,
                        {shaderName, shaderLibs}: InitShaderDataType.initShaderData,
                      ) => {
                        let (vs, fs) =
                          GLSL.buildGLSLSource(shaderLibs, state);

                        let program =
                          gl
                          |> Program.createProgram
                          |> _initShader(vs, fs, gl);

                        state
                        |> GLSLSender.AttributeSendData.addAllData(
                             (shaderName, shaderLibs),
                             gl,
                             program,
                           )
                        |> GLSLSender.UniformSendData.addAllData(
                             (shaderName, shaderLibs),
                             gl,
                             program,
                           )
                        |> GLSLSender.createShaderCacheMap(shaderName)
                        |> Program.setProgram(shaderName, program);
                      },
                      state,
                    )
               )
          )
     );