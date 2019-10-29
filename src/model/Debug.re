open DebugDataType;

let getIsDebug = debugData => debugData.isDebug;

let setIsDebug = (debugData, isDebug) => {
  debugData.isDebug = isDebug;

  debugData;
};