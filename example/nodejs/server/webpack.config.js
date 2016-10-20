const webpack = require('webpack');
const externals = require('webpack-node-externals');

module.exports = {
    entry: './src/index.js',
    output: {
        path: 'bin',
        filename: 'index.js',
    },
    target: 'node',
    externals: [externals()],
    module: {
        loaders: [{
            test: /\.js$/,
            loader: 'babel-loader',
            exclude: /node_modules/,
            query: {
                presets: ['es2015']
            }
        }, {
            test: /\.json$/,
            loader: 'raw-loader',
            exclude: /node_modules/
        }]
    },
    plugins: [
        new webpack.optimize.UglifyJsPlugin({
            compress: {
                warnings: false,
            },
            output: {
                comments: false,
            },
        }),
        new webpack.DefinePlugin({ 'global.GENTLY': false })
    ],
    devtool: '#inline-source-map'
};
