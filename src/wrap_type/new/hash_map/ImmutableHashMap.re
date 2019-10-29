type t('a) = Js.Dict.t(Js.Nullable.t('a));

type t2('key, 'a) = Js.Dict.t(Js.Nullable.t('a));

let createEmpty = HashMap.createEmpty;

let set = (key: string, value: 'a, map: t('a)): t('a) => {
  let newMap = map |> HashMap.copy;

  Js.Dict.set(newMap, key, value |> HashMapType.notNullableToNullable);

  newMap;
};

let get = HashMap.get;

let unsafeGet = HashMap.unsafeGet;

let fastGet = HashMap.fastGet;

/* let length = HashMap.length;

   let fromList = HashMap.fromList;

   let deleteVal = (key: string, map: t('a)): t('a) => {
     let newMap = map |> HashMap.copy;

     Js.Dict.set(newMap, key, Js.Nullable.undefined);

     newMap;
   };


   let entries = HashMap.entries;

    */


   let has = HashMap.has;

let getValidEntries = HashMap.getValidEntries;

/* let getValidValues = HashMap.getValidValues; */

/* let copy = HashMap.copy;

   let map = HashMap.map;

   let mapValid = HashMap.mapValid; */