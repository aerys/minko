const webpack = require('webpack');

module.exports = {
    entry: './src/index.js',
    output: {
        path: '../asset',
        filename: 'index.js',
    },
    target: 'node',
    module: {
        loaders: [{
            test: /\.js$/,
            loader: 'babel-loader'
        }, {
            test: /\.json$/,
            loader: 'raw-loader'
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
