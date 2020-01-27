var gulp = require("gulp");
var path = require("path");

gulp.task("createShaderChunkFile", function (done) {
    var compiler = require("tiny-wonder-glsl-compiler");

    var shaderChunkFilePath = path.join(process.cwd(), "src/glsl/ShaderChunk.re");
    var glslPathArray = [path.join(process.cwd(), "src/glsl/**/*.glsl")];

    compiler.createShaderChunkFile(glslPathArray, shaderChunkFilePath, done);
});