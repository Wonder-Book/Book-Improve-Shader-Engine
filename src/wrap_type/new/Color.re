type r = float;
type g = float;
type b = float;

module Color3 = {
  type t =
    | Color3(r, g, b);

  let create = (r, g, b) => Color3(r, g, b);

  let value = color =>
    switch (color) {
    | Color3(r, g, b) => (r, g, b)
    };
};

module Color4 = {
  type a = float;
  type t =
    | Color4(r, g, b, a);

  let create = (r, g, b, a) => Color4(r, g, b, a);

  let value = color =>
    switch (color) {
    | Color4(r, g, b, a) => (r, g, b, a)
    };
};