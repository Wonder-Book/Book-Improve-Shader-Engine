module Shader = {
  type t =
    | Shader(string);

  let create = value => Shader(value);

  let value = name =>
    switch (name) {
    | Shader(value) => value
    };
};