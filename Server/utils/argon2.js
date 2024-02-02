const argon2 = require('argon2');

class Argon2
{
    static async generateHash(data)
    {
      try {
        const hash = await argon2.hash(data);
        return hash;
      } catch (error) {
        console.error('Error generating hash:', error);
        throw error;
      }
    }

    static async verifyHash(data1, data2)
    { 
      try {
        const match = await argon2.verify(data1, data2);
        return match;
      } catch (error) {
        console.error('Error verifying password:', error);
        throw error;
      }
    }
}

module.exports = Argon2;
