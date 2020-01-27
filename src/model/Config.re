module Render = {
  open RenderConfigDataType;

  open StateDataType;

  let create = ((shaders, shaderLibs)) => Some({shaders, shaderLibs});

  let _unsafeGetData = ({renderConfigData}) =>
    renderConfigData |> Option.unsafeGet;

  let getAllShaders = state =>
    state |> _unsafeGetData |> Result.map(({shaders}) => shaders.shaders);

  let getGroups = state =>
    state |> _unsafeGetData |> Result.map(({shaders}) => shaders.groups);

  let getAllShaderLibs = state =>
    state |> _unsafeGetData |> Result.map(({shaderLibs}) => shaderLibs);

  let _findFirstShaderData =
      (
        shaderLibName: string,
        shaderLibs: array(RenderConfigDataType.shaderLib),
      ) => {
    ContractUtils.requireAndEnsureCheckReturnResult(
      () =>
        ContractUtils.(
          Operators.(
            test(
              LogUtils.buildAssertMessage(
                ~expect={j|shaderLibs.length > 0|j},
                ~actual={j|not|j},
              ),
              () =>
              shaderLibs |> Js.Array.length > 0
            )
          )
        ),
      () =>
        ArrayWT.unsafeFindFirst(
          shaderLibs, shaderLibName, (item: RenderConfigDataType.shaderLib) =>
          item.name === shaderLibName
        ),
      r => {
        open ContractUtils;
        open Operators;

        let arrJson = LogUtils.getJsonStr(shaderLibs);
        let target = shaderLibName;

        test(
          LogUtils.buildAssertMessage(
            ~expect={j|find $target in $arrJson|j},
            ~actual={j|not|j},
          ),
          () =>
          r |> ObjMagicUtils.returnMagicValue |> assertNullableExist
        );
      },
      Debug.getIsDebug(DebugData.getDebugData()),
    );
  };

  let _unsafeFindGroup = (name, groups) =>
    ContractUtils.ensureCheckReturnResult(
      r => {
        open ContractUtils;
        open Operators;

        let arrJson = LogUtils.getJsonStr(groups);
        let target = name;

        test(
          LogUtils.buildAssertMessage(
            ~expect={j|find $target in $arrJson|j},
            ~actual={j|not|j},
          ),
          () =>
          r |> ObjMagicUtils.returnMagicValue |> assertNullableExist
        );
      },
      Debug.getIsDebug(DebugData.getDebugData()),
      ArrayWT.unsafeFindFirst(
        groups, name, (item: RenderConfigDataType.shaderMapData) =>
        item.name === name
      ),
    );

  let _getShaderLibDataArrByGroup =
      (
        groups: array(RenderConfigDataType.shaderMapData),
        name,
        shaderLibs,
        resultDataArr,
      ) => {
    ContractUtils.requireCheckWithResultBodyFuncReturnResult(
      () =>
        ContractUtils.(
          Operators.(
            test(
              LogUtils.buildAssertMessage(
                ~expect={j|groups.length > 0|j},
                ~actual={j|not|j},
              ),
              () =>
              groups |> Js.Array.length > 0
            )
          )
        ),
      () => {
        _unsafeFindGroup(name, groups)
        |> Result.bind(({value}) => {
             value
             |> ArrayWT.traverseResultM((name: string) => {
                  _findFirstShaderData(name, shaderLibs)
                })
           })
        |> Result.map(resultData => {
             Js.Array.concat(resultData, resultDataArr)
           })
      },
      Debug.getIsDebug(DebugData.getDebugData()),
    );
  };

  let _getShaderLibDataArrByType =
      (type_, groups, name, shaderLibs, resultDataArr) =>
    switch (type_) {
    | "group" =>
      _getShaderLibDataArrByGroup(groups, name, shaderLibs, resultDataArr)
    | _ =>
      LogUtils.buildFatalMessage(
        ~title="_getShaderLibDataArrByType",
        ~description={j|unknown type_:$type_|j},
        ~reason="",
        ~solution={j||j},
        ~params={j|shaderLibs: $shaderLibs|j},
      )
      |> ErrorUtils.raiseErrorAndReturn
      |> Result.fail
    };

  let getShaderLibDataArr =
      (
        groups,
        shaderLibItems: array(RenderConfigDataType.shaderLibItem),
        shaderLibs: array(RenderConfigDataType.shaderLib),
      ) =>
    shaderLibItems
    |> TinyWonderCommonlib.ArrayUtils.reduceOneParam(
         (.
           resultDataArrResult,
           {type_, name}: RenderConfigDataType.shaderLibItem,
         ) =>
           resultDataArrResult
           |> Result.bind(resultDataArr => {
                switch (type_) {
                | None =>
                  _findFirstShaderData(name, shaderLibs)
                  |> Result.map(shaderData => {
                       resultDataArr |> ArrayWT.push(shaderData)
                     })

                | Some(type_) =>
                  _getShaderLibDataArrByType(
                    type_,
                    groups,
                    name,
                    shaderLibs,
                    resultDataArr,
                  )
                }
              }),
         ArrayWT.createEmpty() |> Result.succeed,
       );
};