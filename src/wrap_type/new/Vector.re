type t =
  | Vector(float, float, float);

let create = (x, y, z) => Vector(x, y, z);

let value = vec =>
  switch (vec) {
  | Vector(x, y, z) => (x, y, z)
  };

let dot = (v1, v2) => {
  let (x, y, z) = value(v1);
  let (vx, vy, vz) = value(v2);

  x *. vx +. y *. vy +. z *. vz;
};

let sub = (v1, v2) => {
  let (x1, y1, z1) = value(v1);
  let (x2, y2, z2) = value(v2);

  create(x1 -. x2, y1 -. y2, z1 -. z2);
};

let scale = (scalar, v) => {
  let (x, y, z) = value(v);

  create(x *. scalar, y *. scalar, z *. scalar);
};

let cross = (v1, v2) => {
  let (x1, y1, z1) = value(v1);
  let (x2, y2, z2) = value(v2);

  create(y1 *. z2 -. y2 *. z1, z1 *. x2 -. z2 *. x1, x1 *. y2 -. x2 *. y1);
};

let normalize = v => {
  let (x, y, z) = value(v);

  let d = Js.Math.sqrt(x *. x +. y *. y +. z *. z);

  d === 0. ? create(0., 0., 0.) : create(x /. d, y /. d, z /. d);
};

let lengthSq = v => {
  let (x, y, z) = value(v);

  x *. x +. y *. y +. z *. z;
};

let length = vec => Js.Math.sqrt(lengthSq(vec));

let sub = (v1, v2) => {
  let (x1, y1, z1) = value(v1);
  let (x2, y2, z2) = value(v2);

  create(x1 -. x2, y1 -. y2, z1 -. z2);
};

let scale = (scalar, v) => {
  let (x, y, z) = value(v);

  create(x *. scalar, y *. scalar, z *. scalar);
};

let cross = (v1, v2) => {
  let (x1, y1, z1) = value(v1);
  let (x2, y2, z2) = value(v2);

  create(y1 *. z2 -. y2 *. z1, z1 *. x2 -. z2 *. x1, x1 *. y2 -. x2 *. y1);
};

let normalize = v => {
  let (x, y, z) = value(v);

  let d = Js.Math.sqrt(x *. x +. y *. y +. z *. z);
  d === 0. ? create(0., 0., 0.) : create(x /. d, y /. d, z /. d);
};

let lengthSq = v => {
  let (x, y, z) = value(v);

  x *. x +. y *. y +. z *. z;
};

let length = v => Js.Math.sqrt(lengthSq(v));