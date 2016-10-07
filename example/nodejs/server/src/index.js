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
