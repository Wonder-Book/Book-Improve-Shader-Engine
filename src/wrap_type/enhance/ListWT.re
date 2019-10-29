let traverseResultM = (traverseFunc, list) => {
  let returnFunc = Result.succeed;

  list
  |> List.fold_left(
       (resultArr, value) =>
         Result.bind(
           h => Result.bind(t => returnFunc([h, ...t]), resultArr),
           traverseFunc(value),
         ),
       returnFunc([]),
     );
};