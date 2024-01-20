const axios = require('axios');

async function checkCaptchaRequestValid(req)
{
    const response_key = req.body.response_key;
    const secret_key = process.env.RECAPTCHA_SECRET_KEY;
    const options = {
        url: `https://www.google.com/recaptcha/api/siteverify?secret=${secret_key}&response=${response_key}`,
        headers: { "Content-Type": "application/x-www-form-urlencoded", 'json': true }
    }

    let result = false;
    await axios.get(options.url)
    .then(response => {
            result = response.data.success;
    })
    .catch(error => {
        result = false;
    });
    return result;
}

module.exports = { checkCaptchaRequestValid };
