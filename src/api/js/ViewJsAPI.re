let unsafeGetCanvas = state =>
  state |> View.unsafeGetCanvas |> Result.getSuccessValue(ErrorUtils.throwError);

let setCanvasById = View.setCanvasById;