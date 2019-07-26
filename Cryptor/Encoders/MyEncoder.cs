using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Cryptor.Encoders
{
    class MyEncoder
    {
        public static byte[] Encode(byte[] txt, string password)
        {
            byte[] res = new byte[txt.Length];
            byte[] key = Encoding.Unicode.GetBytes(password);

            for (int i = 0; i < txt.Length; i++)
            {
                res[i] = (byte)(txt[i] ^ key[i % key.Length]);
            }

            return res;
        }
    }
}
