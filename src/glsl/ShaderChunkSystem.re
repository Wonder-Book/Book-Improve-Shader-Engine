
  open ShaderChunkType;

  open StateDataType;

  let _getGLSLChunkMap = ({chunkMap}) => chunkMap;

  let getChunk = (name: string, glslChunkData) =>
    glslChunkData
    |> _getGLSLChunkMap
    |> TinyWonderCommonlib.ImmutableHashMap.get(name)
    |> Js.Option.getExn;

  let _buildChunk =
      (
        (top: string, define: string),
        varDeclare: string,
        (funcDeclare: string, funcDefine: string),
        body: string
      ) => {
    top,
    define,
    varDeclare,
    funcDeclare,
    funcDefine,
    body
  };

  let create = () =>
  
    TinyWonderCommonlib.ImmutableHashMap.{
      chunkMap:
        createEmpty()
        
|> set("webgl1_shader2_vertex", _buildChunk(({|

|},{|

|}),{|

|},({|

|},{|

|}),{|
gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vec4(a_position, 1.0);
|}))

|> set("webgl1_shader2_fragment", _buildChunk(({|

|},{|

|}),{|

|},({|

|},{|

|}),{|
gl_FragColor = vec4(u_color0 * u_color1, 1.0);
|}))

|> set("webgl1_shader1_vertex", _buildChunk(({|

|},{|

|}),{|

|},({|

|},{|

|}),{|
gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vec4(a_position, 1.0);
|}))

|> set("webgl1_shader1_fragment", _buildChunk(({|

|},{|

|}),{|

|},({|

|},{|

|}),{|
gl_FragColor = vec4(u_color0, 1.0);
|}))

|> set("webgl1_setPos_mvp", _buildChunk(({|

|},{|

|}),{|

|},({|

|},{|

|}),{|
gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vec4(a_position, 1.0);
|}))

|> set("mediump", _buildChunk(({|
precision mediump float;
precision mediump int;
|},{|

|}),{|

|},({|

|},{|

|}),{|

|}))

|> set("lowp", _buildChunk(({|
precision lowp float;
precision lowp int;
|},{|

|}),{|

|},({|

|},{|

|}),{|

|}))

|> set("highp", _buildChunk(({|
precision highp float;
precision highp int;
|},{|

|}),{|

|},({|

|},{|

|}),{|

|}))

    };
  