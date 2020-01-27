[@bs.send.pipe: array('a)]
external unsafeFindByNull: ('a => bool) => 'a = "find";

let createEmpty = () => [||];

let push = (item, arr) => {
  arr |> Js.Array.push(item) |> ignore;
  arr;
};

let unsafeFindFirstByNull = (arr: array('a), targetValue, func) =>
  arr |> unsafeFindByNull(func);

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