let raiseError = msg => Js.Exn.raiseError(msg) |> ignore;

let raiseErrorAndReturn = msg => Js.Exn.raiseError(msg);

let throwError: Js.Exn.t => unit = [%raw err => {|
throw err;
|}];