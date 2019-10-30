open StateDataType;

let stateData = {state: None};

let getStateData = () => stateData;

let unsafeGetState = () => getStateData().state |> Option.unsafeGet;

let setState = state => {
  getStateData().state = Some(state);

  state;
};