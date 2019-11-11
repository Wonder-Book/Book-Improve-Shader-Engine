open RenderConfigDataType;

let _convertShaderMapData = json =>
  WonderBsJson.Json.(
    Decode.(
      json
      |> array(json =>
           {
             name: json |> field("name", string),
             value: json |> field("value", array(string)),
           }
         )
    )
  );

let convertShadersToRecord = shaders => {
  open WonderBsJson.Json;
  open Decode;

  let json = shaders;
  {
    groups: json |> field("groups", json => _convertShaderMapData(json)),
    shaders:
      json
      |> field("shaders", json =>
           json
           |> array(json =>
                (
                  {
                    name: json |> field("name", string),
                    shaderLibItems:
                      json
                      |> field(
                           "shader_libs",
                           array(json =>
                             (
                               {
                                 type_:
                                   json |> optional(field("type", string)),
                                 name: json |> field("name", string),
                               }: shaderLibItem
                             )
                           ),
                         ),
                  }: shader
                )
              )
         ),
  };
};

let _convertGlslToRecord = json =>
  WonderBsJson.Json.(
    Decode.(
      json
      |> optional(
           field("glsls", json =>
             json
             |> array(json =>
                  {
                    type_: json |> field("type", string),
                    name: json |> field("name", string),
                  }
                )
           ),
         )
    )
  );

let _convertVariableToRecord = json =>
  WonderBsJson.Json.(
    Decode.(
      json
      |> optional(
           field("variables", json =>
             {
               uniforms:
                 json
                 |> optional(
                      field("uniforms", json =>
                        json
                        |> array(json =>
                             {
                               name: json |> field("name", string),
                               field: json |> field("field", string),
                               type_: json |> field("type", string),
                               /* from: json |> field("from", string), */
                             }
                           )
                      ),
                    ),
               attributes:
                 json
                 |> optional(
                      field("attributes", json =>
                        json
                        |> array(json =>
                             {
                               name: json |> optional(field("name", string)),
                               buffer:
                                 json
                                 |> field("buffer", int)
                                 |> intToBufferEnum,
                               type_:
                                 json |> optional(field("type", string)),
                             }
                           )
                      ),
                    ),
             }
           ),
         )
    )
  );

let convertShaderLibsToRecord = shaderLibs =>
  WonderBsJson.Json.(
    Decode.(
      shaderLibs
      |> array(json =>
           {
             name: json |> field("name", string),
             glsls: _convertGlslToRecord(json),
             variables: _convertVariableToRecord(json),
           }
         )
    )
  );