let unsafeGetCanvas = state =>
  state |> View.unsafeGetCanvas |> Result.getSuccessValue(Error.throwError);

let setCanvasById = View.setCanvasById;