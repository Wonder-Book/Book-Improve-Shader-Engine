open StateDataType;

let createDeviceManagerData = () => {
  gl: None,
  clearColor: Color.Color4.create(0., 0., 0., 1.),
};

let setGl = (gl, state) => {
  ...state,
  deviceManagerData: {
    ...state.deviceManagerData,
    gl: Some(gl),
  },
};

let unsafeGetGl = state => state.deviceManagerData.gl |> Option.unsafeGet;

let getClearColor = state => state.deviceManagerData.clearColor;

let setClearColor = (color, state) => {
  ...state,
  deviceManagerData: {
    ...state.deviceManagerData,
    clearColor: color,
  },
};

let clearColor = (gl, state) => {
  let (r, g, b, a) = getClearColor(state) |> Color.Color4.value;

  Gl.clearColor(r, g, b, a, gl);

  state;
};

let clear = gl =>
  Gl.clear(Gl.getColorBufferBit(gl) lor Gl.getDepthBufferBit(gl), gl);

let initGlState = gl => {
  Gl.enable(Gl.getDepthTest(gl), gl);

  Gl.enable(Gl.getCullFace(gl), gl);
  Gl.cullFace(Gl.getBack(gl), gl);
};