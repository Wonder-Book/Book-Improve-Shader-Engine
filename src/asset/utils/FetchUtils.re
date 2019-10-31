open Js.Promise;

open WonderBsMost.Most;

let createFetchJsonStream = filePath =>
  fromPromise(Fetch.fetch(filePath) |> then_(Fetch.Response.json));