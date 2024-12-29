using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Netcore
{
    public static class MessageCodeC
    {
        public static byte[] Encode(DataModel model)
        {
            byte[] message = model.Message;
            int messageLength = message == null ? 0 : message.Length;
            byte[] len = BitConverter.GetBytes(messageLength+2);
            //消息总长度
            byte[] buffer = new byte[messageLength + 2 + 4]; //加上两字节的消息类型和请求类型   4字节的消息长度
                                                             //按照消息封装顺序消息长度+消息类型+消息包的顺序封装
            Buffer.BlockCopy(len, 0, buffer, 0, 4);
            byte[] code = new byte[2] { model.Type, model.Reequest };
            Buffer.BlockCopy(code, 0, buffer, 4, 2);
            //消息包不为空才赋值
            if (message != null)
            {
                Buffer.BlockCopy(message, 0, buffer, 6, messageLength);
            }
            return buffer;
        }
        public static DataModel Decode(byte[]data)
        {
            DataModel model = new DataModel();
            //消息类型
            model.Type = data[0];
            //请求类型
            model.Reequest = data[1];
            //去取出包体数据
            if(data.Length>2)
            {
                byte[] message= new byte[data.Length - 2];
                Buffer.BlockCopy(data, 2, message,0,data.Length-2);
                model.Message = message;
            }
            return model;
        }
    }
}
