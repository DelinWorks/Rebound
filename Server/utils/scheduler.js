const cluster = require('cluster');
const os = require('os');

process.on('message', async (message) => {
    if (message.command === 'queue_spot_proceed')
    {
        try {
            const request = Scheduler.list.shift();
            await request.func(request.req, request.res);
        } catch (err) {}
        finally { process.send({ command: 'queue_spot_finish' }); }
    }
});

class Scheduler
{
    static list = [];
    static workers = [];
    static isParentLocked = false;
    
    static addRequest(func, req, res)
    {
        process.send({ command: 'queue_spot_reserve', worker: cluster.worker.id });
        Scheduler.list.unshift({ func, req, res });
    }

    static setupWorkers()
    {
        for (let i = 0; i < os.cpus().length * 2; i++) {
            const worker = cluster.fork();
            
            Scheduler.workers[worker.id] = worker;

            worker.on('message', async (message) => {
                if (message.command === 'queue_spot_reserve') {
                    Scheduler.list.unshift(message.worker);
                } else if (message.command == 'queue_spot_finish') {
                    Scheduler.isParentLocked = false;
                }
            });
        }
        setInterval(() => { Scheduler.processQueue(); }, 0);
    }

    static processQueue()
    {
        if (Scheduler.list.length > 0 && !Scheduler.isParentLocked)
        {
            Scheduler.isParentLocked = true;
            const worker_id = Scheduler.list.shift();
            Scheduler.workers[worker_id].send({ command: 'queue_spot_proceed' });
        }
    }
}

module.exports = Scheduler;
