let initAll = (contextParam, state) =>
  View.unsafeGetCanvas(state)
  |> Result.bind(canvas => {
       let gl = Gl.getWebgl1Context(canvas, contextParam);
       let state =
         state
         |> DeviceManager.setGl(Gl.getWebgl1Context(canvas, contextParam));

       state
       |> GPUDetect.detect(gl)
       |> Shader.GLSL.setPrecision
       |> Shader.init;
     });

let loopBody = state =>
  DeviceManager.unsafeGetGl(state)
  |> Result.bind(gl => {
       DeviceManager.clearColor(gl, state);

       Gl.clear(Gl.getColorBufferBit(gl) lor Gl.getDepthBufferBit(gl), gl);

       DeviceManager.initGlState(gl);

       Render.render(gl, state);
     });