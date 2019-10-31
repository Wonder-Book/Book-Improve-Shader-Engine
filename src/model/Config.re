module Render = {
  open RenderConfigDataType;

  let create = ((shaders, shaderLibs)) => Some({shaders, shaderLibs});
};