open StateDataType;

let createState = () => {
  deviceManagerData: DeviceManager.createDeviceManagerData(),
  gpuDetectData: GPUDetect.createGPUDetectData(),
  glslData: Shader.GLSL.createGLSLData(),
  glslSenderData: Shader.GLSLSender.createGLSLSenderData(),
  programData: Shader.Program.createProgramData(),
  cameraData: Camera.createCameraData(),
  viewData: View.createViewData(),
  gameObjectData: GameObject.createAllGameObjectData(),
  glslChunkData: ShaderChunk.create(),
  renderConfigData: None,
};