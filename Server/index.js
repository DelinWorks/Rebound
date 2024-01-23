const cluster = require('cluster');
const Scheduler = require('./utils/scheduler.js');

if (cluster.isMaster)
{
    Scheduler.setupWorkers();
    setInterval(() => { Scheduler.processQueue(); }, 0);
}
else
{

require('dotenv').config();

const {checkCaptchaRequestValid} = require('./utils/captcha.js');
const Mailer = require('./utils/mailer.js');
const Argon2 = require('./utils/argon2.js');

const CSRF = require('./utils/csrf_dealer.js');

const Database = require('./app_db.js');
Database.connect();

const express = require('express')
var cookieParser = require('cookie-parser')
var crypto = require('crypto');
const { rateLimit } = require('express-rate-limit')
var validator = require("email-validator");

const Filter = require('bad-words');
const filter = new Filter();

const app = express()
const port = 3000// + cluster.worker.id - 1

app.use(express.json());
app.use(express.urlencoded({ extended: true }));

app.use(cookieParser());
app.use(express.static('public'))
app.set('view engine', 'ejs')

// const limiter = rateLimit({
//     windowMs: 10 * 60 * 1000,
//     limit: 200,
//     standardHeaders: 'draft-7', 
//     legacyHeaders: false,
//     handler: (req, res, next) => {
//         res.status(429).json({ type: "spam", summary: "banned", result: "too many requests.", seat: "" });
//     },
// })

// app.use(limiter)

app.get('/', (req, res) => {
    res.render('front', { pageIndex: 0 });
})

app.get('/create_account', async (req, res) => {
    res.render('front', { pageIndex: 1 });
});

app.post('/create_account', async (req, res) =>
{
    await Scheduler.addRequest(async (req, res) =>
    {
        let valid = true;//await checkCaptchaRequestValid(req);
        let username = req.body.form.username;
        let email = req.body.form.email;

        if (valid !== true)
        {
            res.status(406).send({ type: "input", summary:"error", result:"invalid captcha", seat:'' });
            return;
        }

        if (email.length < 5 || !validator.validate(email))
        {
            res.status(406).send({ type: "input", summary:"error", result:"invalid email", seat:'' });
            return;
        }

        var sql_result;

        await Database.selectFromTable("user", "COUNT(uid) as count, uid, username", "email = ? AND password IS NULL", [email]).then(result => {
            sql_result = result;
        }); if (sql_result[0].count !== 0)
        {
            let uid = sql_result[0].uid;
            var sql_result2;

            await Database.selectFromTable("user_activations", "*", "uid = ?", [uid]).then(result => {
                sql_result2 = result;
            });

            Mailer.sendEmailActivation(email, uid, sql_result[0].username, sql_result2[0].code);

            res.status(201).send({ type: "account_creation", summary:"success", result:"email_resent", seat:`${sql_result[0].username}` });
            return;
        }

        if (username.length === 0)
        {
            res.status(406).send({ type: "input", summary:"error", result:"username too short", seat:'' });
            return;
        }

        if (username.length > 25)
        {
            res.status(406).send({ type: "input", summary:"error", result:`username beyond 25 characters (${username.length})`, seat:'' });
            return;
        }

        if (filter.isProfane(username))
        {
            res.status(406).send({ type: "input", summary:"error", result:`username contains profanity`, seat:'' });
            return;
        }

        const usernameRegex = /^[A-Za-z0-9_]+$/;
        if (!usernameRegex.test(username))
        {
            res.status(406).send({ type: "input", summary:"error", result:"[A-Z a-z 0-9 _] usernames only", seat:'' });
            return;
        }

        await Database.selectFromTable("user", "COUNT(*) as count", "username = ?", [username]).then(result => {
            sql_result = result;
        }); if (sql_result[0].count !== 0)
        {
            res.status(406).send({ type: "input", summary:"error", result:"username taken", seat:'' });
            return;
        }

        await Database.selectFromTable("user", "COUNT(*) as count", "email = ?", [email]).then(result => {
            sql_result = result;
        }); if (sql_result[0].count !== 0)
        {
            res.status(406).send({ type: "input", summary:"error", result:"email already used", seat:'' });
            return;
        }

        data = {
            username: username,
            email: email
        }

        await Database.insertIntoTable("user", data).then(result => {
            sql_result = result;
            console.log(sql_result)
        })

        let uid = sql_result.insertId;
        let code = await Argon2.generateHash(email + username + process.env.ACCOUNT_CREATION_TOKEN_HASH_SALT);

        data = {
            uid: uid,
            code: code
        }

        await Database.insertIntoTable("user_activations", data).then(result => {
            sql_result = result;
        })

        if (sql_result == undefined)
        {
            await Database.deleteFromTable("user", "uid = ?", [uid]);
            res.status(406).send({ type: "input", summary:"error", result:"activation token failed", seat:'' });
            return;
        }

        Mailer.sendEmailActivation(email, uid, username, code);

        res.status(201).send({ type: "account_creation", summary:"success", result:"redirect", seat:'' });
    }, req, res);
})

app.get('/reset_password', (req, res) => {
    res.render('front', { pageIndex: 2 });
})

app.get('/email_2fa', (req, res) => {
    res.render('front', { pageIndex: 3 });
})

app.get('/email_2fa', (req, res) => {
    res.render('front', { pageIndex: 3 });
})

app.get('/change_account_email', (req, res) => {
    res.render('front', { pageIndex: 4 });
})

app.get('/delete_account', (req, res) => {
    res.render('front', { pageIndex: 5 });
})

app.get('/song_bucket', (req, res) => {
    res.render('front', { pageIndex: 6 });
})

app.get('/account_created', (req, res) => {
    res.render('front', { pageIndex: 7, email_resent: (req.query.email_resent === undefined), username: req.query.u }); 
})

app.get('/verify', (req, res) => {

})

app.listen(port, () => console.log(`worker ${cluster.worker.id} webapp: listening on port ${port}`))

}
