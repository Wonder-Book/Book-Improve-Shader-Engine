open Js.Typed_array;

module Vertices = {
  type t =
    | Vertices(Float32Array.t);

  let create = value => Vertices(value);

  let value = vertices =>
    switch (vertices) {
    | Vertices(value) => value
    };
};

module Indices = {
  type t =
    | Indices(Uint16Array.t);

  let create = value => Indices(value);

  let value = indices =>
    switch (indices) {
    | Indices(value) => value
    };

  let bind = (f, indices) => indices |> value |> f;

  let length = indices => indices |> bind(Uint16Array.length);
};