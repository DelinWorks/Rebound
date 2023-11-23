const express = require('express')
var cookieParser = require('cookie-parser')
const crypto = require('crypto')
const emailValidator = require('email-validator')
const shell = require('shelljs')

const app = express()
const app_db = require('./app_db')
const { resourceLimits } = require('worker_threads')

function mail(fromname, email, subject, html) {
    shell.exec(`python3 ./mail_service.py '${fromname}' '${email}' '${subject}' '${html}'`)
}

app_db.query('USE delingames', function (err, result) {
    if (err) throw err;
});

//app_db.query(`SELECT COUNT(*) as count FROM tasktt_user_table`, function (err, result) {
    //if (err) throw err;
    //console.log(result[0]['count'])
//});

app.use(cookieParser());
app.use(express.static('public'))
app.set('view engine', 'ejs')

function makeid(length, salt) {
    var result           = '';
    var characters       = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789`~!@#$%^&*-+=';
    var charactersLength = characters.length;
    for ( var i = 0; i < length; i++ ) {
        result += characters.charAt(Math.floor(Math.random() * charactersLength));
    }
    return crypto.createHash('MD5').update(result + salt).digest("hex");
}

const interval = setInterval(function() {
    update()
}, 10000);

function update() {
    app_db.query('SELECT * FROM tasktt_tasks WHERE reminder=1 AND is_done=0', function (err, result) {
        if (err) {
            res.sendStatus(500)
            return;
        }

        result.forEach(element => {
            taskdate = new Date(element.datetime)
            nowdate = new Date()
            nowdate = nowdate.setHours(nowdate.getHours() + element.timezone_offset);

            if (taskdate < nowdate)
                app_db.query('SELECT email FROM tasktt_users WHERE hash=?', [element.hash], function (err, result) {
                    if (err) {
                        res.sendStatus(500)
                        return;
                    }

                    email = result[0].email
                    app_db.query('SELECT DATE_FORMAT (datetime,\'%d/%m/%Y %h:%i %p\') AS datetime FROM tasktt_tasks WHERE id=?', [element.id], function (err, result) {
                        if (err) {
                            res.sendStatus(500)
                            return;
                        }

                        datetimeStr = result[0]['datetime']
                        
                        app_db.query('UPDATE tasktt_tasks SET is_done=1 WHERE id=?', [element.id], function (err, result) {
                            if (err) {
                                res.sendStatus(500)
                                return;
                            }

                            subject = 'You got a task to do!'
                            html = `<h3>You got a task at ${datetimeStr} called ${element.task_name}. Get Ready!</h3>`
                            try { 
                                mail(element.task_name, email, subject, html)
                            } catch (err) {}
                    })
                })
            })
        });
    })
}

app.get('/', (req, res) => {
    has_key = req.cookies.user_session !== undefined;
    app_db.query('SELECT COUNT(*) as count FROM tasktt_tasks WHERE is_done=0', function (err, result) {
        if (err) {
            res.sendStatus(500)
            return;
        }

        res.render('tasktt', {
            has_key: has_key,
            key: req.cookies.user_session,
            count: result[0]['count'],
        })
        
    });
})

app.get('/login', (req, res) => {
    email = req.query.email
    if (email !== undefined && emailValidator.validate(email))
    {
        email = email.toLowerCase();
        app_db.query('SELECT hash FROM tasktt_users where email=?', [email], function (err, result) {
            if (err) {
                res.sendStatus(500)
                return;
            }
            
            if (result.length == 0) {
                key = makeid(16, email);
                app_db.query('INSERT INTO tasktt_users VALUES (?, ?)', [key, email], function (err, result) {
                    if (err) {
                        res.sendStatus(500)
                        return;
                    }
                    res.cookie('user_session', key, { maxAge: 3600 * 1000 * 24 * 30 * 7 });
                    res.redirect('/')
                })
            } else {
                res.cookie('user_session', result[0]['hash'], { maxAge: 3600 * 1000 * 24 * 30 * 7 });
                res.redirect('/')
            }
        })
    } else {
        res.clearCookie('user_session')
        res.redirect('/')
    }
})

app.get('/validate', (req, res) => {
    key = req.cookies.user_session;
    if (key === undefined || !key.match(/^[a-zA-Z0-9_]{8,}$/))
    {
        res.send('INVALID_PARAMETER')
        return;
    }

    app_db.query('SELECT * FROM tasktt_users where hash=?', [key], function (err, result) {
        if (err) {
            res.sendStatus(500)
            return;
        }
        if (result.length == 0)
        {
            res.send('EMAIL_NOT_FOUND')
            return;
        } else {
            res.send(result[0].email)
            return;
        }
    })
})

app.get('/list', (req, res) => {
    key = req.cookies.user_session;
    console.log(key);
    app_db.query('SELECT * FROM tasktt_users where hash=?', [key], function (err, result) {
        if (err) {
            res.sendStatus(500)
            return;
        }
        if (result.length == 0) {
            res.send('unknown error occured')
            return;
        } else {

            app_db.query('SELECT id, hash, timezone_offset, task_name, DATE_FORMAT (datetime,\'%d/%m/%Y %h:%i %p\') AS datetime, reminder FROM tasktt_tasks WHERE hash=? AND is_done=0 ORDER BY reminder DESC, UNIX_TIMESTAMP(datetime) ASC',

            [key], function (err, result) {
                res.send(JSON.stringify(result))
                return
            })
        }
    })
})

app.get('/add', (req, res) => {
    key = req.cookies.user_session;

    timezone_offset = req.query.timezone_offset
    task_name = req.query.task_name
    datetime = req.query.datetime
    reminder = req.query.reminder == 'true' ? 1 : 0

    if (task_name.length == 0) { res.send('task_name cannot be empty'); return }
    if (datetime.length == 0) { res.send('datetime cannot be empty'); return }

    app_db.query('SELECT * FROM tasktt_users where hash=?', [key], function (err, result) {
        if (err) {
            res.sendStatus(500)
            return;
        }
        if (result.length == 0) {
            res.send('unknown error occured')
            return;
        } else {
            app_db.query('SELECT * FROM tasktt_tasks where hash=? AND task_name=? AND is_done=0', [key, task_name], function (err, result) {
                if (err) {
                    res.sendStatus(500)
                    return
                }
                
                if (result.length == 0)
                {
                    app_db.query('INSERT INTO tasktt_tasks(hash, timezone_offset, task_name, datetime, reminder) VALUES (?, ?, ?, ?, ?)', [key, timezone_offset, task_name, datetime, reminder], function (err, result) {
                        if (err) {
                            res.sendStatus(500)
                            return
                        }
                        res.send('OK')
                    })
                } else {
                    res.send('this task already exists')
                    return
                }
            })
        }
    })
})

app.get('/toggle', (req, res) => {
    key = req.cookies.user_session
    id = req.query.id
    app_db.query('SELECT reminder FROM tasktt_tasks where id=? AND hash=?', [id, key], function (err, result) {
        if (err) {
            res.sendStatus(500)
            return
        }
        if (result.length == 0) {
            res.send('task not available or you don\'t own it')
            return
        } else {
            reminder = result[0].reminder == 0 ? 1 : 0
            app_db.query('UPDATE tasktt_tasks SET reminder=? where id=? AND hash=?', [reminder, id, key], function (err, result) {
                if (err) {
                    res.sendStatus(500)
                    return
                }
                
                res.send('OK')
                return
            })
        }
    })
})

app.get('/remove', (req, res) => {
    key = req.cookies.user_session
    id = req.query.id
    app_db.query('SELECT reminder FROM tasktt_tasks where id=? AND hash=?', [id, key], function (err, result) {
        if (err) {
            res.sendStatus(500)
            return
        }
        if (result.length == 0) {
            res.send('task not available or you don\'t own it')
            return
        } else {
            app_db.query('UPDATE tasktt_tasks SET is_done=1 where id=? AND hash=?', [id, key], function (err, result) {
                if (err) {
                    res.sendStatus(500)
                    return
                }
                
                res.send('OK')
                return
            })
        }
    })
})

app.listen(8080)
