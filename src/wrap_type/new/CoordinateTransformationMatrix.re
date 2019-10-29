module type CoordinateTransformationMatrix = {
  type t = ..;

  let getMatrixValue: (t => Matrix.t, t) => Js.Typed_array.Float32Array.t;
};

module CoordinateTransformationMatrix = {
  type t = ..;

  let getMatrixValue = (getValueFunc, coordinateTransformationMatrix) =>
    coordinateTransformationMatrix |> getValueFunc |> Matrix.value;
};

module MakeModel = (Item: CoordinateTransformationMatrix) => {
  type Item.t +=
    | Model(Matrix.t);

  type t = Item.t;

  let create = mat => Model(mat);

  let value = mMat =>
    switch (mMat) {
    | Model(mat) => mat
    };

  let getMatrixValue = coordinateTransformationMatrix =>
    coordinateTransformationMatrix |> Item.getMatrixValue(value);
};

module MakeView = (Item: CoordinateTransformationMatrix) => {
  type Item.t +=
    | View(Matrix.t);

  type t = Item.t;

  let create = mat => View(mat);

  let value = vMat =>
    switch (vMat) {
    | View(mat) => mat
    };

  let getMatrixValue = coordinateTransformationMatrix =>
    coordinateTransformationMatrix |> Item.getMatrixValue(value);
};

module MakeProjection = (Item: CoordinateTransformationMatrix) => {
  type Item.t +=
    | Projection(Matrix.t);

  type t = Item.t;

  let create = mat => Projection(mat);

  let value = pMat =>
    switch (pMat) {
    | Projection(mat) => mat
    };

  let getMatrixValue = coordinateTransformationMatrix =>
    coordinateTransformationMatrix |> Item.getMatrixValue(value);
};

module Model = MakeModel(CoordinateTransformationMatrix);

module View = MakeView(CoordinateTransformationMatrix);

module Projection = MakeProjection(CoordinateTransformationMatrix);