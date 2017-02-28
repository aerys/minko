require('source-map-support').install();
require('mongoose-nedb').install();

const path = require('path');
const express = require('express');
const superagent = require('superagent');
const mongoose = require('mongoose');
const echo = require('./echo');

const Datastore = require('nedb');

mongoose.Promise = require('bluebird');

echo.echo("hello world");

let app = express();

mongoose.connect('mongodb://localhost/counters');
mongoose.connection.on('error', (e) => console.error('Mongoose connection error:', e));
mongoose.connection.once('open', () => console.log('Mongoose connected'));

var counterSchema = new mongoose.Schema({
    name: String,
    value: Number
});

let Counter = mongoose.model('Counter', counterSchema);

app.get('/hello', (req, res) => {
    Counter.update({ name: 'my-counter' }, { $inc: { value: 1 } }, { upsert: true }, (error) => {
        if (error)
            res.json({ 'success': false, 'error': error });
        else
            Counter.find({ name: 'my-counter' }, (error, documents) =>
            {
                if (error)
                    res.json({ 'success': false, 'error': error });
                else
                    res.json({ 'success': true, 'counter': documents[0].value });

                // counter.persistence.compactDatafile();
            });
    });

});

app.listen(3000, () => {
    console.log('Example app listening on port 3000!');

    // Call itself.
    superagent
        .get('http://localhost:3000/hello')
        .end((err, res) => {
            if (err)
                console.log(`Got error: ${err.status}`);
            else
                console.log(`Got response: ${res.status}`);
        });
});
