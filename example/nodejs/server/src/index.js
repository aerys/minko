require("source-map-support").install();

const express = require('express');
const superagent = require('superagent');
const echo = require('./echo');

superagent
    .get('http://www.google.com/index.html')
    .end((err, res) => {
        if (err)
            console.log(`Got error: ${err.status}`);
        else
            console.log(`Got response: ${res.status}`);
    });

echo.echo("hello world");

let app = express();
let counter = 0;

app.get('/hello', (req, res) => res.json({ 'success': true, 'counter': ++counter }));

app.listen(3000, () => console.log('Example app listening on port 3000!'));
