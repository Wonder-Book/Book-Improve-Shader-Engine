/* type initShaderData = {
     shaderName: string,
     vs: string,
     fs: string,
     attributeFieldNameList: list(string),
     uniformFieldNameList: list(string),
   }; */

type initShaderData = {
  shaderName: string,
  shaderLibs: array(RenderConfigDataType.shaderLib),
};