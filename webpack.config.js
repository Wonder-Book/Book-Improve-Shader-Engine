const path = require('path');

const isProd = process.env.NODE_ENV === 'production';

module.exports = {
  entry: {
      "wd": "./lib/es6_global/src/Index.js"
  },
  mode: isProd ? 'production' : 'development',
  output: {
      filename: '[name].js',
      path: path.join(__dirname, "dist"),
      library: 'wd',
      libraryTarget: 'umd'
  },
  target: "web"
};