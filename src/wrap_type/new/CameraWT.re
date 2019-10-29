module Frustum = {
  module Fovy = {
    type t =
      | Fovy(float);

    let create = value => Fovy(value);

    let value = fovy =>
      switch (fovy) {
      | Fovy(value) => value
      };
  };

  module Aspect = {
    type t =
      | Aspect(float);

    let create = value => Aspect(value);

    let value = aspect =>
      switch (aspect) {
      | Aspect(value) => value
      };
  };

  module Near = {
    type t =
      | Near(float);

    let create = value => Near(value);

    let value = near =>
      switch (near) {
      | Near(value) => value
      };
  };

  module Far = {
    type t =
      | Far(float);

    let create = value => Far(value);

    let value = far =>
      switch (far) {
      | Far(value) => value
      };
  };
};