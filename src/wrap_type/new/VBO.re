module VertexBuffer = {
  type t =
    | VertexBuffer(Gl.buffer);

  let create = value => VertexBuffer(value);

  let value = vbo =>
    switch (vbo) {
    | VertexBuffer(value) => value
    };
};

module IndexBuffer = {
  type t =
    | IndexBuffer(Gl.buffer);

  let create = value => IndexBuffer(value);

  let value = vbo =>
    switch (vbo) {
    | IndexBuffer(value) => value
    };
};