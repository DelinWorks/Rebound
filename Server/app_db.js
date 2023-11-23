const mysql = require('mysql')

var connection = mysql.createConnection({
    host: "localhost",
    user: "client",
    password: "~censored~"
});

connection.connect(function(err) {
    if (err) throw err;
    console.log("mysql: connected as client");
});

module.exports = connection
