[@bs.send.pipe: array('a)]
external unsafeFindByNull: ('a => [@bs.uncurry] bool) => 'a = "find";

let createEmpty = () => [||];

let push = (item, arr) => {
  arr |> Js.Array.push(item) |> ignore;
  arr;
};

// let unsafeGetFirst = arr => Array.unsafe_get(arr, 0);

let unsafeFindFirstByNull = (arr: array('a), targetValue, func) =>
  arr |> unsafeFindByNull(func);
/* |> WonderLog.Contract.ensureCheck(
     first => {
       open WonderLog;
       open Contract;
       open Operators;
       let arrJson = WonderLog.Log.getJsonStr(arr);
       test(
         Log.buildAssertMessage(
           ~expect={j|find $targetValue in $arrJson|j},
           ~actual={j|not|j},
         ),
         () =>
         first |> assertNullableExist
       );
     },
     IsDebugMainService.getIsDebug(StateDataMain.stateData),
   ); */

let traverseResultM = (traverseFunc, arr) => {
  let returnFunc = Result.succeed;

  arr
  |> TinyWonderCommonlib.ArrayUtils.reduceOneParam(
       (. resultArr, value) =>
         Result.bind(
           h => Result.bind(t => returnFunc(push(h, t)), resultArr),
           traverseFunc(value),
         ),
       returnFunc([||]),
     );
};