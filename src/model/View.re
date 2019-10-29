open StateDataType;

let createViewData = () => {canvas: None};

let unsafeGetCanvas = state => state.viewData.canvas |> Option.unsafeGet;

let _setCanvas = (canvas, state) => {
  ...state,
  viewData: {
    canvas: Some(canvas),
  },
};

let setCanvasById = (canvasId, state) =>
  _setCanvas(
    DomExtend.querySelector(DomExtend.document, {j|#$canvasId|j}),
    state,
  );