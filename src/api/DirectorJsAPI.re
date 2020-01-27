let initAll = (contextParam, state) =>
  Director.initAll(contextParam, state)
  |> Result.getSuccessValue(ErrorUtils.throwError);

let rec _loop = () =>
  DomExtend.requestAnimationFrame((time: float) => {
    StateData.unsafeGetState()
    |> Result.bind(Director.loopBody)
    |> Result.tap(state => state |> StateData.setState)
    |> Result.getSuccessValue(ErrorUtils.throwError)
    |> ignore;

    _loop() |> ignore;
  });

let loop = state => {
  StateData.setState(state) |> ignore;

  _loop();
};