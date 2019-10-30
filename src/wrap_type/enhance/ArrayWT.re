let push = (item, arr) => {
  arr |> Js.Array.push(item) |> ignore;
  arr;
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