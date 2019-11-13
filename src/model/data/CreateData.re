open StateDataType;

let createState = () => {
  deviceManagerData: DeviceManager.createDeviceManagerData(),
  gpuDetectData: GPUDetect.createGPUDetectData(),
  glslData: Shader.GLSL.createGLSLData(),
  glslSenderData: Shader.GLSLSender.createGLSLSenderData(),
  programData: Shader.Program.createProgramData(),
  cameraData: Camera.createCameraData(),
  viewData: View.createViewData(),
  allGameObjectData: GameObject.createAllGameObjectData(),
  glslChunkData: ShaderChunkSystem.create(),
  renderConfigData: None,
};