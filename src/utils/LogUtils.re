let log = msg => Js.log({j|$msg|j});

let getJsonStr = json => Js.Json.stringify(json |> Obj.magic);

let buildAssertMessage = (~expect, ~actual) => {j|expect $expect, but actual $actual|j};

let buildFatalMessage = (~title, ~description, ~reason, ~solution, ~params) => {j|
  Fatal:

  title
  $title

  description
  $description

  reason
  $reason

  solution
  $solution

  params
  $params

   |j};