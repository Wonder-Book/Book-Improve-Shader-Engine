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

  let _findFirstShaderDataByThrow =
      (
        shaderLibName: string,
        shaderLibs: array(RenderConfigDataType.shaderLib),
      ) => {
    ContractUtils.requireCheck(
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
      Debug.getIsDebug(DebugData.getDebugData()),
    );

    ArrayWT.unsafeFindFirst(
      shaderLibs, shaderLibName, (item: RenderConfigDataType.shaderLib) =>
      item.name === shaderLibName
    )
    |> ContractUtils.ensureCheck(
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

  let _unsafeFindGroupByThrow = (name, groups) =>
    ArrayWT.unsafeFindFirst(
      groups, name, (item: RenderConfigDataType.shaderMapData) =>
      item.name === name
    )
    |> ContractUtils.ensureCheck(
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
       );

  let _getShaderLibDataArrByGroupByThrow =
      (
        groups: array(RenderConfigDataType.shaderMapData),
        name,
        shaderLibs,
        resultDataArr,
      ) => {
    ContractUtils.requireCheck(
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
      Debug.getIsDebug(DebugData.getDebugData()),
    );

    Js.Array.concat(
      _unsafeFindGroupByThrow(name, groups).value
      |> Js.Array.map((name: string) =>
           _findFirstShaderDataByThrow(name, shaderLibs)
         ),
      resultDataArr,
    );
  };

  let _getShaderLibDataArrByTypeByThrow =
      /* (
           materialIndex,
           type_,
           groups: array( shaderMapData),
           name,
           isSourceInstance,
           isSupportInstance,
         ),
         (
           shaderLibs,
           staticBranchs: array(shaderMapData),
           dynamicBranchs,
           state,
         ), */
      /* (getMaterialShaderLibDataArrByStaticBranchFunc, isPassFunc), */
      /* resultDataArr, */
      (type_, groups, name, shaderLibs, resultDataArr) =>
    switch (type_) {
    | "group" =>
      _getShaderLibDataArrByGroupByThrow(
        groups,
        name,
        shaderLibs,
        resultDataArr,
      )
    /* | "static_branch" =>
         getMaterialShaderLibDataArrByStaticBranchFunc(.
           (name, isSourceInstance, isSupportInstance),
           (staticBranchs: array(shaderMapData), shaderLibs),
           resultDataArr,
         )
       | "dynamic_branch" =>
         getMaterialShaderLibDataArrByDynamicBranch(
           (materialIndex, name),
           (dynamicBranchs, shaderLibs, state),
           isPassFunc,
           resultDataArr,
         ) */
    | _ =>
      ErrorUtils.raiseErrorAndReturn(
        LogUtils.buildFatalMessage(
          ~title="_getShaderLibDataArrByTypeByThrow",
          ~description={j|unknown type_:$type_|j},
          ~reason="",
          ~solution={j||j},
          ~params={j|shaderLibs: $shaderLibs|j},
        ),
      )
    };

  let getShaderLibDataArr =
      (
        groups,
        shaderLibItems: array(RenderConfigDataType.shaderLibItem),
        shaderLibs: array(RenderConfigDataType.shaderLib),
      ) =>
    shaderLibItems
    |> TinyWonderCommonlib.ArrayUtils.reduceOneParam(
         (. resultDataArr, {type_, name}: RenderConfigDataType.shaderLibItem) =>
           switch (type_) {
           | None =>
             resultDataArr
             |> ArrayWT.push(_findFirstShaderDataByThrow(name, shaderLibs))
           | Some(type_) =>
             _getShaderLibDataArrByTypeByThrow(
               /* (
                    materialIndex,
                    type_ |> OptionService.unsafeGetJsonSerializedValue,
                    groups,
                    name,
                    isSourceInstance,
                    isSupportInstance,
                  ),
                  (shaderLibs, staticBranchs, dynamicBranchs, state),
                  (getMaterialShaderLibDataArrByStaticBranchFunc, isPassFunc),
                  resultDataArr, */
               type_,
               groups,
               name,
               shaderLibs,
               resultDataArr,
             )
           },
         ArrayWT.createEmpty(),
       );
};