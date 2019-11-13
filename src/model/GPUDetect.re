open StateDataType;

open GPUDetectType;

let createGPUDetectData = () => {vao: None, precision: None};

let _detectExtension = (gl, state) => {
  ...state,
  gpuDetectData: {
    ...state.gpuDetectData,
    vao:
      gl
      |> Gl.getExtension("OES_vertex_array_object")
      |> Js.Nullable.toOption
      |> Js.Option.map((. ext) => ext |> GPUDetectType.glExtensionToVAOExt),
  },
};

let _detectPrecision = (gl, state) => {
  open Gl;

  /* TODO handle Some experimental-webgl implementations do not have getShaderPrecisionFormat:

     if (!gl.getShaderPrecisionFormat) {
         record.precision = EGPUPrecision.HIGHP;

         return;
     } */

  let vertexShader = gl |> getVertexShader;
  let fragmentShader = gl |> getFragmentShader;
  let highFloat = gl |> getHighFloat;
  let mediumFloat = gl |> getMediumFloat;
  let vertexShaderPrecisionHighpFloat =
    gl |> getShaderPrecisionFormat(vertexShader, highFloat);
  let vertexShaderPrecisionMediumpFloat =
    gl |> getShaderPrecisionFormat(vertexShader, mediumFloat);
  let fragmentShaderPrecisionHighpFloat =
    gl |> getShaderPrecisionFormat(fragmentShader, highFloat);
  let fragmentShaderPrecisionMediumpFloat =
    gl |> getShaderPrecisionFormat(fragmentShader, mediumFloat);
  let highpAvailable =
    vertexShaderPrecisionHighpFloat##precision > 0
    &&
    fragmentShaderPrecisionHighpFloat##precision > 0;
  let mediumpAvailable =
    vertexShaderPrecisionMediumpFloat##precision > 0
    &&
    fragmentShaderPrecisionMediumpFloat##precision > 0;

  let precision =
    if (!highpAvailable) {
      if (mediumpAvailable) {
        LogUtils.warn({j|not support highp, using mediump instead|j});

        MEDIUMP;
      } else {
        LogUtils.warn(
          {j|not support highp and mediump, using lowp instead|j},
        );

        LOWP;
      };
    } else {
      HIGHP;
    };

  {
    ...state,
    gpuDetectData: {
      ...state.gpuDetectData,
      precision: Some(precision),
    },
  };
};

let detect = (gl, state) =>
  state |> _detectExtension(gl) |> _detectPrecision(gl);

let unsafeGetVAOExt = state => state.gpuDetectData.vao |> Option.unsafeGet;