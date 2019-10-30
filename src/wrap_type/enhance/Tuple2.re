let sequenceResultM = ((aResult, bResult)) => {
  let returnFunc = Result.succeed;

  aResult |> Result.bind(a => bResult |> Result.map(b => (a, b)));
};