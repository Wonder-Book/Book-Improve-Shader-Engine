let isEmpty = value =>
  Obj.magic(value) === Js.Nullable.null
  || Obj.magic(value) === Js.Nullable.undefined;