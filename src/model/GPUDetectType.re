type vaoExt = {
  .
  [@bs.meth] "createVertexArrayOES": unit => Gl.vao,
  [@bs.meth] "bindVertexArrayOES": Js.Nullable.t(Gl.vao) => unit,
};

external glExtensionToVAOExt: Gl.extension => vaoExt = "%identity";