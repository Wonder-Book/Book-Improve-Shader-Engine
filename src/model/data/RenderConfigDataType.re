type shaderMapData = {
  name: string,
  value: array(string),
};

type shaderLibItem = {
  type_: option(string),
  name: string,
};

type shader = {
  name: string,
  shaderLibs: array(shaderLibItem),
};

type shaders = {
  groups: array(shaderMapData),
  shaders: array(shader),
};

type glsl = {
  type_: string,
  name: string,
};

type bufferEnum =
  | Vertex
  | Index;

type attribute = {
  name: option(string),
  buffer: bufferEnum,
  type_: option(string),
};

type uniform = {
  name: string,
  field: string,
  type_: string,
  /* from: string, */
};

type variables = {
  uniforms: option(array(uniform)),
  attributes: option(array(attribute)),
};

type shaderLib = {
  name: string,
  glsls: option(array(glsl)),
  variables: option(variables),
};

type shaderLibs = array(shaderLib);

type renderConfigData = {
  shaders,
  shaderLibs,
};

external intToBufferEnum: int => bufferEnum = "%identity";