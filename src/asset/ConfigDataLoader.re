open Js.Promise;

open WonderBsMost.Most;

let _createFetchRenderConfigStreamArr = configDir => [|
  FetchUtils.createFetchJsonStream(
    PathUtils.join([|configDir, "render/shader/shaders.json"|]),
  )
  |> map(json => ParseRenderConfig.convertShadersToRecord(json))
  |> Obj.magic,
  FetchUtils.createFetchJsonStream(
    PathUtils.join([|configDir, "render/shader/shader_libs.json"|]),
  )
  |> map(json => ParseRenderConfig.convertShaderLibsToRecord(json))
  |> Obj.magic,
|];

let _collectAllRecords = stream =>
  stream |> reduce((arr, record) => arr |> ArrayWT.push(record), [||]);

let load = configDir => {
  let state = CreateData.createState();

  fromPromise(
    _createFetchRenderConfigStreamArr(configDir)
    |> MostUtils.concatArray
    |> _collectAllRecords
    |> then_(recordArr =>
         {
           ...state,
           renderConfigData: Config.Render.create(recordArr |> Obj.magic),
         }
         |> StateData.setState
         |> resolve
       ),
  );
};