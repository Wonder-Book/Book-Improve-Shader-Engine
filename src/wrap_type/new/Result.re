type t('a, 'b) =
  | Success('a)
  | Fail('b);

let succeed = x => Success(x);

let fail = x => Fail(x);

let either = (successFunc, failureFunc, twoTrackInput) =>
  switch (twoTrackInput) {
  | Success(s) => successFunc(s)
  | Fail(f) => failureFunc(f)
  };

let bind = (switchFunc, twoTrackInput) =>
  either(switchFunc, fail, twoTrackInput);

let tap = (oneTrackFunc, twoTrackInput) =>
  either(
    result => {
      result |> oneTrackFunc |> ignore;
      result |> succeed;
    },
    fail,
    twoTrackInput,
  );

let map = (oneTrackFunc, twoTrackInput) =>
  either(result => result |> oneTrackFunc |> succeed, fail, twoTrackInput);

/*
 let apply =
     (
       ~switchFunc,
       ~result,
       ~addFailureFunc,
       ~handleFailAndSuceessFunc,
       ~handleSuceessFuncAndFailFunc,
     ) =>
   switch (switchFunc, result) {
   | (Success(func), Success(s)) => Success(func(s))
   | (Fail(f1), Success(s)) => Fail(handleFailAndSuceessFunc(f1, s))
   | (Success(func), Fail(f2)) =>
     Fail(handleSuceessFuncAndFailFunc(func, f2))
   | (Fail(f1), Fail(f2)) => Fail(addFailureFunc(f1, f2))
   }; */

let tryCatch = (oneTrackFunc: 'a => 'b, x: 'a): t('b, Js.Exn.t) =>
  try (oneTrackFunc(x) |> succeed) {
  | Js.Exn.Error(e) => fail(e)
  | err => fail(Js.Exn.raiseError({j|unknown error: $err|j}))
  };

let mapSuccess = (mapFunc, result) =>
  switch (result) {
  | Success(s) => mapFunc(s) |> succeed
  | Fail(f) => fail(f)
  };

let getSuccessValue = (handleFailFunc: 'f => unit, result: t('s, 'f)): 's =>
  switch (result) {
  | Success(s) => s
  | Fail(f) => handleFailFunc(f) |> ObjMagicUtils.returnMagicValue
  };