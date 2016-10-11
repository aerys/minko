require("source-map-support").install();

const path = require('path');
const express = require('express');
const superagent = require('superagent');
const echo = require('./echo');

const Datastore = require('nedb');

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

let counter = new Datastore({ filename: path.dirname(process.argv[1]) + '/counter.db', autoload: true });

app.get('/hello', (req, res) => {
    counter.update({ name: 'my-counter' }, { $inc: { value: 1 } }, { upsert: true }, (error) => {
        if (error)
            res.json({ 'success': false, 'error': error });
        else
            counter.find({ name: 'my-counter' }, (error, documents) =>
            {
                if (error)
                    res.json({ 'success': false, 'error': error });
                else
                    res.json({ 'success': true, 'counter': documents[0].value })

                counter.persistence.compactDatafile();
            });
    });
});

app.listen(3000, () => console.log('Example app listening on port 3000!'));
