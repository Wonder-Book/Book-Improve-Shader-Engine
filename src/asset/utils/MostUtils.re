open WonderBsMost.Most;

let _isFromEventStream = [%bs.raw
  stream => {|
    var source = stream.source;
    return !!source.event && !!source.source;
  |}
];

let concatArray = streamArr =>
  switch (Js.Array.length(streamArr)) {
  | 0 => WonderBsMost.Most.just(Obj.magic(1))
  | _ =>
    streamArr
    |> Js.Array.sliceFrom(1)
    |> TinyWonderCommonlib.ArrayUtils.reduceOneParam(
         (. stream1, stream2) =>
           _isFromEventStream(stream1) === true ?
             stream1 |> concat(stream2) : stream1 |> concat(stream2),
         streamArr[0],
       )
  };