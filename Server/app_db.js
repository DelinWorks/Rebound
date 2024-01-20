const cluster = require('cluster');
const mysql = require('mysql')

class Database
{
    static database = 0;
 
    static connect()
    {
        Database.database = mysql.createConnection({
            host: "127.0.0.0",
            user: "player",
            password: process.env.MYSQL_PLAYER_PASSWORD,
            database: "rebound"
        });

        Database.database.connect(function(err) { if (err) console.log(err); console.log(`worker ${cluster.worker.id} mysql: connected as player`); });
    }

    static async selectFromTable(tableName, columns, clause = undefined, extra = undefined) {
        if (clause != undefined)
            clause = "WHERE " + clause;
        return new Promise((resolve, reject) => {
            Database.database.query(`SELECT ${columns} FROM ${tableName} ${clause}`, extra, (err, result) => {
                if (err) {
                    console.error(err);
                    resolve(undefined);
                } else {
                    resolve(result);
                }
            });
        });
    }

    static async insertIntoTable(tableName, extra) {
        return new Promise((resolve, reject) => {
            Database.database.query(`INSERT INTO ${tableName} SET ?`, extra, (err, result) => {
                if (err) {
                    console.error(err);
                    resolve(undefined);
                } else {
                    resolve(result);
                }
            });
        });
    }

    static async updateTable(tableName, set, clause, extra = undefined) {
        clause = "WHERE " + clause;
        return new Promise((resolve, reject) => {
            Database.database.query(`UPDATE ${tableName} SET ${set} ${clause}`, extra, (err, result) => {
                if (err) {
                    console.error(err);
                    resolve(undefined);
                } else {
                    resolve(result);
                }
            });
        });
    }

    static async deleteFromTable(tableName, clause, extra = undefined) {
        clause = "WHERE " + clause;
        return new Promise((resolve, reject) => {
            Database.database.query(`DELETE FROM ${tableName} ${clause}`, extra, (err, result) => {
                if (err) {
                    console.error(err);
                    resolve(undefined);
                } else {
                    resolve(result);
                }
            });
        });
    }
}

module.exports = Database
