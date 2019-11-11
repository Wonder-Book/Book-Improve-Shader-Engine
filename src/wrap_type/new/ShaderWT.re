module Shader = {
  type t =
    | Shader(string);

  let create = value => Shader(value);

  let value = name =>
    switch (name) {
    | Shader(value) => value
    };
};

module FieldName = {
  type t =
    | FieldName(string);

  let create = value => FieldName(value);

  let value = name =>
    switch (name) {
    | FieldName(value) => value
    };
};