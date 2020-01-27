let _assert = (result: bool, message) =>
  !result ? ErrorUtils.raiseError(message) : ();

let test = (message, func) =>
  try(func()) {
  | _ => ErrorUtils.raiseError(message)
  };

let requireCheckReturnResult =
    (checkFunc: unit => unit, bodyFunc: unit => 'a, isCheck: bool)
    : Result.t('a, Js.Exn.t) =>
  isCheck
    ? try(
        {
          checkFunc();
          bodyFunc() |> Result.succeed;
        }
      ) {
      | Js.Exn.Error(e) => Result.fail(e)
      | err =>
        Result.fail(
          ErrorUtils.raiseErrorAndReturn({j|unknown check error: $err|j}),
        )
      }
    : bodyFunc() |> Result.succeed;

let ensureCheckReturnResult =
    (checkFunc: 'a => unit, isCheck: bool, returnVal: 'a)
    : Result.t('a, Js.Exn.t) =>
  isCheck
    ? try(
        {
          checkFunc(returnVal);
          returnVal |> Result.succeed;
        }
      ) {
      | Js.Exn.Error(e) => Result.fail(e)
      | err =>
        Result.fail(
          ErrorUtils.raiseErrorAndReturn({j|unknown check error: $err|j}),
        )
      }
    : returnVal |> Result.succeed;

let requireAndEnsureCheckReturnResult =
    (
      requireCheckFunc: unit => unit,
      bodyFunc: unit => 'a,
      ensureCheckFunc: 'a => unit,
      isCheck: bool,
    )
    : Result.t('a, Js.Exn.t) =>
  isCheck
    ? try(
        {
          requireCheckFunc();

          let returnVal = bodyFunc();

          ensureCheckFunc(returnVal);
          returnVal |> Result.succeed;
        }
      ) {
      | Js.Exn.Error(e) => Result.fail(e)
      | err =>
        Result.fail(
          ErrorUtils.raiseErrorAndReturn({j|unknown check error: $err|j}),
        )
      }
    : bodyFunc() |> Result.succeed;

let requireCheck = (checkFunc: unit => unit, isCheck: bool): unit =>
  isCheck ? checkFunc() : ();

let ensureCheck = (checkFunc: 'a => unit, isCheck: bool, returnVal: 'a): 'a =>
  isCheck
    ? {
      checkFunc(returnVal);
      returnVal;
    }
    : returnVal;

let assertFailWithMessage = message => ErrorUtils.raiseError(message);

let assertFail = () => assertFailWithMessage("fail");

let assertPass = () => ();

let assertTrue = (source: bool) =>
  _assert(source == true, "expect to be true, but actual is false");

let assertFalse = (source: bool) =>
  _assert(source == false, "expect to be false, but actual is true");

let assertJsTrue = (source: bool) =>
  _assert(source == true, "expect to be true, but actual is false");

let assertJsFalse = (source: bool) =>
  _assert(source == false, "expect to be false, but actual is true");

let assertIsBool = (source: bool) =>
  _assert(
    source == true || source == false,
    "expect to be bool, but actual not",
  );

let assertNullableExist = (source: 'a) =>
  _assert(
    !TinyWonderCommonlib.NullUtils.isEmpty(source),
    "expect exist, but actual not",
  );

let assertNullExist = (source: 'a) =>
  _assert(!Js.Null.test(source), "expect exist, but actual not");

let assertExist = (source: option('a)) =>
  _assert(Js.Option.isSome(source), "expect exist, but actual not");

let assertNotExist = (source: option('a)) =>
  _assert(Js.Option.isNone(source), "expect not exist, but actual exist");

type assertEqual(_) =
  | Int: assertEqual(int)
  | Float: assertEqual(float)
  | String: assertEqual(string);

let _getEqualMessage = (source, target) => {j|"expect to be $target, but actual is $source"|j};

let assertEqual = (type g, kind: assertEqual(g), source: g, target: g) =>
  switch (kind) {
  | _ => _assert(source == target, _getEqualMessage(source, target))
  };

type assertNotEqual(_) =
  | Int: assertNotEqual(int)
  | Float: assertNotEqual(float)
  | String: assertNotEqual(string);

let _getNotEqualMessage = (source, target) => {j|"expect not to be $target, but actual is $source"|j};

let assertNotEqual = (type g, kind: assertNotEqual(g), source: g, target: g) =>
  switch (kind) {
  | _ => _assert(source != target, _getNotEqualMessage(source, target))
  };

type assertNumber(_) =
  | Int: assertNumber(int)
  | Float: assertNumber(float);

let assertGt = (type g, kind: assertNumber(g), source: g, target: g) =>
  switch (kind) {
  | _ =>
    _assert(source > target, {j|expect $source > $target, but actual isn't|j})
  };

let assertGte = (type g, kind: assertNumber(g), source: g, target: g) =>
  switch (kind) {
  | _ =>
    _assert(
      source >= target,
      {j|expect $source >= $target, but actual isn't|j},
    )
  };

let assertLt = (type g, kind: assertNumber(g), source: g, target: g) =>
  switch (kind) {
  | _ =>
    _assert(source < target, {j|expect $source < $target, but actual isn't|j})
  };

let assertLte = (type g, kind: assertNumber(g), source: g, target: g) =>
  switch (kind) {
  | _ =>
    _assert(
      source <= target,
      {j|expect $source <= $target, but actual isn't|j},
    )
  };

module Operators = {
  let (==) = (a, b) => assertEqual(Int, a, b);
  let (==.) = (a, b) => assertEqual(Float, a, b);
  let (==^) = (a, b) => assertEqual(String, a, b);
  let (<>=) = (a, b) => assertNotEqual(Int, a, b);
  let (<>=.) = (a, b) => assertNotEqual(Float, a, b);
  let (>) = (a, b) => assertGt(Int, a, b);
  let (>.) = (a, b) => assertGt(Float, a, b);
  let (>=) = (a, b) => assertGte(Int, a, b);
  let (>=.) = (a, b) => assertGte(Float, a, b);
  let (<) = (a, b) => assertLt(Int, a, b);
  let (<.) = (a, b) => assertLt(Float, a, b);
  let (<=) = (a, b) => assertLte(Int, a, b);
  let (<=.) = (a, b) => assertLte(Float, a, b);
};