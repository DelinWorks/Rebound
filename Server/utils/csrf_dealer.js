const crypto = require('crypto');

class CSRF
{
    static CSRF_ACCOUNT_VERIFICATION_SALT = process.env.CSRF_ACCOUNT_VERIFICATION_SALT

    static generateToken(req, salt)
    {
        var ip = req.headers['x-forwarded-for'] || req.socket.remoteAddress
        var hash = crypto.createHash('sha256');
        hash.update(ip + salt);
        const hashedData = hash.digest('hex');
        return hashedData;
    }

    static compareToken(req, hashstr, salt)
    {
        var ip = req.headers['x-forwarded-for'] || req.socket.remoteAddress
        var hash = crypto.createHash('sha256');
        hash.update(ip + salt);
        const hashedData = hash.digest('hex');
        return hashstr == hashedData;
    }
}

module.exports = CSRF;
