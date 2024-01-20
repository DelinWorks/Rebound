const nodemailer = require('nodemailer');

class Mailer
{
    static send(to, subject, html)
    {
        const transporter = nodemailer.createTransport({
            host: '127.0.0.0',
            port: 25,
            tls: { rejectUnauthorized: false, },
            });
            
            const mailOptions = {
            from: 'smtp@delingames.top',
            to: to,
            subject: subject,
            html: html,
            };
            
            transporter.sendMail(mailOptions, (error, info) => {
            if (error)
                console.error('Error sending email:', error);
            console.log('Email sent:', info.response);
            });
    }

    static sendEmailActivation(email, uid, username, code, csrf)
    {
        Mailer.send(email, `Welcome To Rebound ${username}`,
        `<h2>Welcome To Rebound ${username} !</h2>
        <h3>Please Activate Your Account to get the most out of Rebound Game!<h3><br>
        <h3>Here's Your Activation Link https://delingames.top/verify?uid=${uid}&usrnm=${username}&hash=${code}&csrf=${csrf}</h3><h4>You can set your password from there!</h4><br>
        <h4>Much love ~ <3</h4>
        `);
    }
}

module.exports = Mailer;
