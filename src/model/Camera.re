open StateDataType;

let createCameraData = () => {vMatrix: None, pMatrix: None};

let unsafeGetVMatrix = state => state.cameraData.vMatrix |> Option.unsafeGet;

let unsafeGetPMatrix = state => state.cameraData.pMatrix |> Option.unsafeGet;

let setCameraData = ((vMatrix, pMatrix), state) => {
  ...state,
  cameraData: {
    vMatrix: Some(vMatrix),
    pMatrix: Some(pMatrix),
  },
};