open StateDataType;

module GLSL = {
  let createGLSLData = () => {glslMap: ImmutableHashMap.createEmpty()};

  let _getGLSLMap = state => state.glslData.glslMap;

  let _setGLSLMap = (glslMap, state) => {
    ...state,
    glslData: {
      ...state.glslData,
      glslMap,
    },
  };

  let addGLSL = (shaderName, glslData, state) =>
    _setGLSLMap(
      _getGLSLMap(state)
      |> ImmutableHashMap.set(
           ShaderWT.ShaderName.value(shaderName),
           glslData,
         ),
      state,
    );

  let getAllValidGLSLEntries = state =>
    _getGLSLMap(state) |> ImmutableHashMap.getValidEntries;

  let getAllValidGLSLEntryList = state =>
    state |> getAllValidGLSLEntries |> Array.to_list;
};

module Program = {
  let createProgramData = () => {
    programMap: ImmutableHashMap.createEmpty(),
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
    Contract.requireCheckByThrow(
      () =>
        Contract.(
          Operators.(
            test(
              Log.buildAssertMessage(
                ~expect={j|program of shader:$shaderName exist|j},
                ~actual={j|not|j},
              ),
              () =>
              _getProgramMap(state)
              |> ImmutableHashMap.has(shaderName)
              |> assertTrue
            )
          )
        ),
      Debug.getIsDebug(DebugData.getDebugData()),
    );

    _getProgramMap(state) |> ImmutableHashMap.unsafeGet(shaderName);
  };

  let setProgram = (shaderName, program, state) =>
    _setProgramMap(
      _getProgramMap(state) |> ImmutableHashMap.set(shaderName, program),
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
  let createGLSLLocationData = () => {
    attributeLocationMap: ImmutableHashMap.createEmpty(),
    uniformLocationMap: ImmutableHashMap.createEmpty(),
  };

  let _getOrCreateLocationMapOfShader = (shaderName, locationMap) =>
    switch (locationMap |> ImmutableHashMap.get(shaderName)) {
    | None => ImmutableHashMap.createEmpty()
    | Some(locationMapOfShader) => locationMapOfShader
    };

  let _getAttributeLocationMap = state =>
    state.glslLocationData.attributeLocationMap;

  let unsafeGetAttribLocation = (shaderName, fieldName, state) => {
    Contract.requireCheckByThrow(
      () =>
        Contract.(
          Operators.(
            test(
              Log.buildAssertMessage(
                ~expect=
                  {j|attrib field: $fieldName exist in shader:$shaderName|j},
                ~actual={j|not|j},
              ),
              () => {
                _getAttributeLocationMap(state)
                |> ImmutableHashMap.has(shaderName)
                |> assertTrue;

                _getAttributeLocationMap(state)
                |> ImmutableHashMap.unsafeGet(shaderName)
                |> ImmutableHashMap.has(fieldName)
                |> assertTrue;
              },
            )
          )
        ),
      Debug.getIsDebug(DebugData.getDebugData()),
    );

    _getAttributeLocationMap(state)
    |> ImmutableHashMap.unsafeGet(shaderName)
    |> ImmutableHashMap.unsafeGet(fieldName);
  };

  let _getAttribLocation = (program, fieldName, gl) =>
    Gl.getAttribLocation(program, fieldName, gl)
    |> Contract.ensureCheckByThrow(
         location =>
           Contract.(
             Operators.(
               test(
                 Log.buildAssertMessage(
                   ~expect={j|attrib location of $fieldName exist|j},
                   ~actual={j|not|j},
                 ),
                 () =>
                 location <>= (-1)
               )
             )
           ),
         Debug.getIsDebug(DebugData.getDebugData()),
       );

  let setAttribLocation = (program, shaderName, fieldName, gl, state) => {
    let attributeLocationMap = _getAttributeLocationMap(state);
    let location = _getAttribLocation(program, fieldName, gl);

    {
      ...state,
      glslLocationData: {
        ...state.glslLocationData,
        attributeLocationMap:
          attributeLocationMap
          |> _getOrCreateLocationMapOfShader(shaderName)
          |> ImmutableHashMap.set(fieldName, location)
          |> ImmutableHashMap.set(shaderName, _, attributeLocationMap),
      },
    };
  };

  let _getUniformLocationMap = state =>
    state.glslLocationData.uniformLocationMap;

  let unsafeGetUniformLocation = (shaderName, fieldName, state) => {
    Contract.requireCheckByThrow(
      () =>
        Contract.(
          Operators.(
            test(
              Log.buildAssertMessage(
                ~expect=
                  {j|uniform field: $fieldName exist in shader:$shaderName|j},
                ~actual={j|not|j},
              ),
              () => {
                _getUniformLocationMap(state)
                |> ImmutableHashMap.has(shaderName)
                |> assertTrue;

                _getUniformLocationMap(state)
                |> ImmutableHashMap.unsafeGet(shaderName)
                |> ImmutableHashMap.has(fieldName)
                |> assertTrue;
              },
            )
          )
        ),
      Debug.getIsDebug(DebugData.getDebugData()),
    );

    _getUniformLocationMap(state)
    |> ImmutableHashMap.unsafeGet(shaderName)
    |> ImmutableHashMap.unsafeGet(fieldName);
  };

  let _getUniformLocation = (program, fieldName, gl) =>
    Gl.getUniformLocation(program, fieldName, gl)
    |> Contract.ensureCheckByThrow(
         location =>
           Contract.(
             Operators.(
               test(
                 Log.buildAssertMessage(
                   ~expect={j|uniform location of $fieldName exist|j},
                   ~actual={j|not|j},
                 ),
                 () =>
                 Obj.magic(location) |> assertNullExist
               )
             )
           ),
         Debug.getIsDebug(DebugData.getDebugData()),
       );

  let setUniformLocation = (program, shaderName, fieldName, gl, state) => {
    let uniformLocationMap = _getUniformLocationMap(state);
    let location =
      _getUniformLocation(program, fieldName, gl) |> Js.Null.getUnsafe;

    {
      ...state,
      glslLocationData: {
        ...state.glslLocationData,
        uniformLocationMap:
          uniformLocationMap
          |> _getOrCreateLocationMapOfShader(shaderName)
          |> ImmutableHashMap.set(fieldName, location)
          |> ImmutableHashMap.set(shaderName, _, uniformLocationMap),
      },
    };
  };
};

module GLSLSender = {
  let createGLSLSenderData = () => {
    uniformCacheMap: ImmutableHashMap.createEmpty(),
    lastBindedVAO: None,
  };

  let _fastGetCache = (shaderCacheMap, name: string) =>
    shaderCacheMap |> ImmutableHashMap.fastGet(name);

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
    shaderCacheMap |> ImmutableHashMap.set(name, record);

  let _isNotCacheVector3AndSetCache =
      (shaderCacheMap, name: string, (x: float, y: float, z: float)) => {
    let (has, cache) = _fastGetCache(shaderCacheMap, name);

    has ?
      (shaderCacheMap, _queryIsNotCacheWithCache(cache, x, y, z)) :
      (_setCache(shaderCacheMap, name, [|x, y, z|]), true);
  };

  let sendFloat3 =
      (gl, (name: string, pos: Gl.uniformLocation), valueArr, shaderCacheMap) => {
    Contract.requireCheckByThrow(
      () =>
        Contract.(
          Operators.(
            test(
              Log.buildAssertMessage(
                ~expect={j|valueArr.length === 3|j},
                ~actual={j|not|j},
              ),
              () =>
              valueArr |> Js.Array.length == 3
            )
          )
        ),
      Debug.getIsDebug(DebugData.getDebugData()),
    );

    let x = valueArr[0];
    let y = valueArr[1];
    let z = valueArr[2];

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
    Contract.requireCheckByThrow(
      () =>
        Contract.(
          Operators.(
            test(
              Log.buildAssertMessage(
                ~expect={j|shaderCacheMap of shader:$shaderName exist|j},
                ~actual={j|not|j},
              ),
              () =>
              getUniformCacheMap(state)
              |> ImmutableHashMap.has(shaderName)
              |> assertTrue
            )
          )
        ),
      Debug.getIsDebug(DebugData.getDebugData()),
    );

    getUniformCacheMap(state) |> ImmutableHashMap.unsafeGet(shaderName);
  };

  let setShaderCacheMap = (shaderName, shaderCacheMap, state) =>
    getUniformCacheMap(state)
    |> ImmutableHashMap.set(shaderName, shaderCacheMap)
    |> setUniformCacheMap(_, state);

  let createShaderCacheMap = (shaderName, state) =>
    getUniformCacheMap(state)
    |> ImmutableHashMap.set(shaderName, ImmutableHashMap.createEmpty())
    |> setUniformCacheMap(_, state);

  let getLastSendedVAO = state => state.glslSenderData.lastBindedVAO;

  let setLastSendedVAO = (lastBindedVAO, state) => {
    ...state,
    glslSenderData: {
      ...state.glslSenderData,
      lastBindedVAO: Some(lastBindedVAO),
    },
  };
};

let _compileShader = (gl, glslSource: string, shader) => {
  Gl.shaderSource(shader, glslSource, gl);
  Gl.compileShader(shader, gl);

  shader
  |> Contract.ensureCheckByThrow(
       shader =>
         Contract.(
           Gl.getShaderParameter(shader, Gl.getCompileStatus(gl), gl)
           === false ?
             {
               let message = Gl.getShaderInfoLog(shader, gl);

               Log.log({j|shader info log:
          $message|j});
               Log.log({j|glsl source:
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
  |> Contract.ensureCheckByThrow(
       () =>
         Contract.
           /*! perf: slow

             let message = Gl.getProgramInfoLog(program, gl);

             test(
               Log.buildAssertMessage(
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

                 Log.buildAssertMessage(
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

let _changeGLSLDataListToInitShaderDataList = glslDataList =>
  glslDataList
  |> List.map(
       (
         (
           shaderName,
           ((vs, fs), attributeFieldNameList, uniformFieldNameList),
         ),
       ) =>
       (
         {
           shaderName,
           vs: GLSLWT.VS.value(vs),
           fs: GLSLWT.FS.value(fs),
           attributeFieldNameList:
             attributeFieldNameList
             |> List.map(fieldName => ShaderWT.FieldName.value(fieldName)),
           uniformFieldNameList:
             uniformFieldNameList
             |> List.map(fieldName => ShaderWT.FieldName.value(fieldName)),
         }: InitShaderDataType.initShaderData
       )
     );

let init =
    (state: StateDataType.state): Result.t(StateDataType.state, Js.Exn.t) =>
  DeviceManager.unsafeGetGl(state)
  |> Result.bind(gl =>
       GLSL.getAllValidGLSLEntryList(state)
       |> _changeGLSLDataListToInitShaderDataList
       |> Result.tryCatch(initShaderDataList =>
            initShaderDataList
            |> List.fold_left(
                 (
                   state,
                   {
                     shaderName,
                     vs,
                     fs,
                     attributeFieldNameList,
                     uniformFieldNameList,
                   }: InitShaderDataType.initShaderData,
                 ) => {
                   let program =
                     gl |> Program.createProgram |> _initShader(vs, fs, gl);

                   let state =
                     attributeFieldNameList
                     |> List.fold_left(
                          (state, fieldName) =>
                            state
                            |> GLSLLocation.setAttribLocation(
                                 program,
                                 shaderName,
                                 fieldName,
                                 gl,
                               ),
                          state,
                        );
                   let state =
                     uniformFieldNameList
                     |> List.fold_left(
                          (state, fieldName) =>
                            state
                            |> GLSLLocation.setUniformLocation(
                                 program,
                                 shaderName,
                                 fieldName,
                                 gl,
                               ),
                          state,
                        );

                   state
                   |> GLSLSender.createShaderCacheMap(shaderName)
                   |> Program.setProgram(shaderName, program);
                 },
                 state,
               )
          )
     );