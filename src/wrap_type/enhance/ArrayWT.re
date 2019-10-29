let push = (item, arr) => {
  arr |> Js.Array.push(item) |> ignore;
  arr;
};

let reduceOneParam = (func, param, arr) => {
  let mutableParam = ref(param);
  for (i in 0 to Js.Array.length(arr) - 1) {
    mutableParam := func(. mutableParam^, Array.unsafe_get(arr, i));
  };
  mutableParam^;
};
/* 
let traverseResultM = (traverseFunc, arr) => {
  let returnFunc = Result.succeed;

  arr
  |> reduceOneParam(
       (. resultArr, value) =>
         Result.bind(
           h => Result.bind(t => returnFunc(push(h, t)), resultArr),
           traverseFunc(value),
         ),
       returnFunc([||]),
     );
}; */