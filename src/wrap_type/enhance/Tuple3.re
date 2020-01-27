let sequenceResultM = ((aResult, bResult, cResult)) => {
  let returnFunc = Result.succeed;

  aResult
  |> Result.bind(a =>
       bResult |> Result.bind(b => cResult |> Result.map(c => (a, b, c)))
     );
};