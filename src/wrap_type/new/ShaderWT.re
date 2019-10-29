module ShaderName = {
  type t =
    | ShaderName(string);

  let create = value => ShaderName(value);

  let value = name =>
    switch (name) {
    | ShaderName(value) => value
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