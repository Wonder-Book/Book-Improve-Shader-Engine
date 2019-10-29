module VS = {
  type t =
    | VS(string);

  let create = value => VS(value);

  let value = vs =>
    switch (vs) {
    | VS(value) => value
    };
};

module FS = {
  type t =
    | FS(string);

  let create = value => FS(value);

  let value = vs =>
    switch (vs) {
    | FS(value) => value
    };
};