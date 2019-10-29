open StateDataType;

open GPUDetectType;

let createGPUDetectData = () => {vao: None};

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

let detect = (gl, state) => state |> _detectExtension(gl);

let unsafeGetVAOExt = state => state.gpuDetectData.vao |> Option.unsafeGet;