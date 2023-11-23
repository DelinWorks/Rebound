const cluster = require('cluster');
const os = require('os');

const Redis = require('ioredis');
const { request } = require('http');
const redis = new Redis(6379, 'localhost');
// redis.set('mutex', 'free');

const sleep = ms => new Promise(r => setTimeout(r, ms));
// const blockUntilLockAquired = (async () => { while (!await acquireRedisLock()) { sleep(15); }});
// const releaseAquiredLockSync = (async () => { releaseRedisLockSync(); });

// const criticalSectionExecuteOrBlock = async (func, ...params) => {
//     try {
//       await blockUntilLockAquired();
//       await func(...params);
//     } catch (err) {}
//     finally {
//         await releaseAquiredLockSync();
//         //process.send({ command: 'signal_mutex' });
//     }
//   };

// const criticalSectionExecuteOrQueue = async (func, req, res) => {
//     let isLockAquired = false;
//     try {
//         if (await acquireRedisLock())
//         {
//             isLockAquired = true;
//             await func(req, res);
//         }
//         else
//         {
//             requestList.push({ func, req, res });
//         }
//     } catch (err) {}
//     finally 
//     {
//         if (isLockAquired)
//             await releaseRedisLockSync();
//     }
// };

// async function acquireRedisLock()
// {
//     const multi = redis.multi();
//     multi.get('mutex');
//     multi.set('mutex', 'acquired', 'EX', 10);
//     const results = await multi.exec();
//     const currentLockStatus = results[0][1];
    
//     if (currentLockStatus !== 'free')
//     {
//         await redis.set('mutex', 'acquired', 'EX', 10);
//         return false;
//     }

//     return true;
// }

  
// async function releaseRedisLockSync() {
//     await redis.set('mutex', 'free');
// }

const requestList = [];

const criticalSectionQueue = async (func, req, res) => {
    process.send({ command: 'queue_spot_reserve', worker: cluster.worker.id });
    requestList.unshift({ func, req, res });
};

process.on('message', async (message) => {
    if (message.command === 'queue_spot_proceed')
    {
        try {
            const request = requestList.shift();
            await request.func(request.req, request.res);
        } catch (err) {}
        finally { process.send({ command: 'queue_spot_finish' }); }
    }
});

let isParentLocked = false;

if (cluster.isMaster) {
    const workers = {};
    //os.cpus().length * 1
    for (let i = 0; i < os.cpus().length * 1; i++) {
        const worker = cluster.fork();
        
        workers[worker.id] = worker;
        worker.on('message', async (message) => {
            if (message.command === 'queue_spot_reserve') {
                requestList.unshift(message.worker);
            } else if (message.command == 'queue_spot_finish') {
                isParentLocked = false;
            }
        });
    }

    async function processQueuedCriticalJobs()
    {
        if (requestList.length > 0 && !isParentLocked)
        {
            isParentLocked = true;
            const worker_id = requestList.shift();
            workers[worker_id].send({ command: 'queue_spot_proceed' });
        }
    }

    setInterval(() => { processQueuedCriticalJobs(); }, 0);
} else
{

const express = require('express')
var cookieParser = require('cookie-parser')
const axios = require('axios');
var crypto = require('crypto');
const { rateLimit } = require('express-rate-limit')
var validator = require("email-validator");

const svgCaptcha = require('svg-captcha');
const { time } = require('console');

const Filter = require('bad-words');
const filter = new Filter();

const app = express()
const port = 3000// + cluster.worker.id - 1

app.use(express.json());
app.use(express.urlencoded({ extended: true }));

app.use(cookieParser());
app.use(express.static('public'))
app.set('view engine', 'ejs')

// async function processQueuedCriticalJobs()
// {
//     if (requestList.length > 0)
//     {
//         let isLockAquired = false;
//         try {
//             if (await acquireRedisLock())
//             {
//                 isLockAquired = true;
//                 const request = await requestList.pop();
//                 await request.func(request.req, request.res);
//             }
//         } catch (err) {}
//         finally {
//             if (isLockAquired)
//                 await releaseRedisLockSync();
//         }
//     }
// }

// setInterval(() => { processQueuedCriticalJobs(); }, 15);

// process.on('message', (message) => {
//     if (message.command === 'process_jobs') {
//     }
// });

const limiter = rateLimit({
    windowMs: 10 * 60 * 1000,
    limit: 200,
    standardHeaders: 'draft-7', 
    legacyHeaders: false,
    handler: (req, res, next) => {
        res.status(429).json({ type: "spam", summary: "banned", result: "too many requests.", seat: "" });
    },
})

// Apply the rate limiting middleware to all requests
app.use(limiter)

// app.get('/captcha', (req, res) => {
//     req.query.rateLimit =   false;
//     svgCaptcha.loadFont('public/positions-regular.otf');
//     svgCaptcha.options.fontSize = 48
//     svgCaptcha.options.charPreset = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789'
//     const captcha = svgCaptcha.create({
//         size: 5,        // Number of characters
//         noise: 4,       // Amount of noise lines
//         color: true,
//         background: '#fff'
//     });

// // const client = createClient();
// // client.connect();

// // // client.set('mykey', 'Hello from node redis');
// // const myKeyValue = client.get('192.168.0.0');
// // console.log(myKeyValue);

// if (req.cookies && req.cookies.captcha)
// {
//     const cookieValue = req.cookies.captcha
//     redis.set('captcha:' + cookieValue, captcha.text)
// } else {
//     uuid = req.get('User-Agent') + Math.floor(+new Date() / 1000)
//     uuid = crypto.createHash('md5').update(uuid).digest("hex")

//     res.cookie('captcha', uuid)
//     redis.set('captcha:' + uuid, captcha.text)
// }

// res.type('svg');
// res.status(200).send(captcha.data);
// });

app.get('/', (req, res) => {
    res.render('front', { pageIndex: 0 });
})

app.get('/create_account', async (req, res) => {
    res.render('front', { pageIndex: 1 });
});

async function isCaptchaRequestValid(req, res)
{
    // if (req.cookies.captcha === undefined || req.body === undefined || req.body.captcha_token === undefined || req.body.captcha_token.length < 5)
    // {
    //     res.status(406).send({ type: "captcha", summary:"error", result:"invalid captcha", seat:'' });
    //     return -1;
    // }

    // shouldReturn = false;
    // await redis.get('captcha:' + req.cookies.captcha, (err, result) => {
    //     if (err)
    //     {
    //         res.status(406).send({ type: "captcha", summary:"error", result:"invalid captcha", seat:'' });
    //         shouldReturn = true;
    //         return;
    //     } else
    //     {
    //         if (result === null) {
    //             res.status(406).send({ type: "captcha", summary:"error", result:"invalid captcha", seat:'' });
    //             shouldReturn = true;
    //             return;
    //         } else if (result.length === 0)
    //         {
    //             res.status(406).send({ type: "captcha", summary:"error", result:"expired captcha", seat:'' });
    //             shouldReturn = true;
    //             return;
    //         }
    //         else if (result !== req.body.captcha_token)
    //         {
    //             res.status(406).send({ type: "captcha", summary:"error", result:"incorrect captcha", seat:'' });
    //             shouldReturn = true;
    //             return;
    //         }
    //     }
    // });

    // if (shouldReturn)
    //     return -1;
    
    // redis.set('captcha:' + req.cookies.captcha, '');
    // return 0;



    return 0;
}

app.get('/block', async (req, res) => {
    await criticalSectionExecuteOrQueue(async () => {
        const result = performBlockingOperation();
        res.send(`Result: ${result}`);
    });
});
  
function performBlockingOperation() {
    let result = 0;
    for (let i = 0; i < 9000000000; i++) {
        result += i;
    }
    return result;
}

const createAccountPostHandler = async (req, res) => {
    // await sleep(1000);
    const result = await isCaptchaRequestValid(req, res);
    if (result === 0)
    {
        let username = req.body.username;
        let email = req.body.email;

        if (email.length < 5 || !validator.validate(email))
        {
            res.status(406).send({ type: "input", summary:"error", result:"invalid email", seat:'' });
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

        res.status(201).send({ type: "account_creation", summary:"success", result:"redirect", seat:'' });
    }
};

app.post('/create_account', async (req, res) =>
{
    await criticalSectionQueue(createAccountPostHandler, req, res);
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

app.listen(port, () => console.log(`web app listening on port ${port}!`))

}
