using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Threading;

public class SocketClient
{
    private static SocketClient instance;

    public static SocketClient Instance
    { 
        get
        {
            if(instance==null)
            {
                instance = new SocketClient();
            }
            return instance;
        }
    }

    private Socket client;
    private IPEndPoint hostEndPoint;
    //线程同步
    private AutoResetEvent connectResultEvent;
    //异步接收对象
    private SocketAsyncEventArgs receiveAsync;
    //发送对象池
    private Stack<SocketAsyncEventArgs> sendAsyncPool;
    private int poolCapacity;
    //接受缓存区
    private List<byte> receiveBuffer;

    //连接结果委托
    public delegate void OnConnectResult(bool result);
    public OnConnectResult ConnectResultEvent;
    //断开服务器委托
    public delegate void OnDisconnect();
    public OnDisconnect DisconnectEvent;
    //接收数据委托
    public delegate void OnDataReceive(byte[] data);
    public OnDataReceive DataReceiveEvent;

    /// <summary>
    /// 是否连接服务器
    /// </summary>

    public bool Connected
    {
        get { return client != null && client.Connected; }
    }
    public void ConnectServer(string ip,int port)
    {
        if (Connected) return;
        //实例化
        hostEndPoint = new IPEndPoint(IPAddress.Parse(ip), port);
        //实例化socket(这里可能可以考虑使用UDP
        client = new Socket(hostEndPoint.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
        
        //实现线程同步
        connectResultEvent = new AutoResetEvent(false);
        //异步对象,实例化SocketAsyncEventArgs
        SocketAsyncEventArgs connectAsync = new SocketAsyncEventArgs();
        connectAsync.UserToken = client;
        connectAsync.RemoteEndPoint = hostEndPoint;
        connectAsync.Completed += connectResult;

        //实例化对象发送池
        sendAsyncPool = new Stack<SocketAsyncEventArgs>();
        //实例化缓存区
        receiveBuffer = new List<byte>();
        //异步连接
        client.ConnectAsync(connectAsync);
        //阻塞主线程5s,为连接预留时间
        connectResultEvent.WaitOne(5000);
        //连接结果委托调用
        ConnectResultEvent?.Invoke(Connected);//问号的作用是检测是否为空
        if(Connected)
        {
            //断开连接用到的bool
            isConnected = true;
            //连接成功后实例化接受对象
            receiveAsync = new SocketAsyncEventArgs();
            receiveAsync.RemoteEndPoint = hostEndPoint;
            receiveAsync.Completed += ReceiveCompleted;
            byte[] buffer = new byte[1024];

            receiveAsync.SetBuffer(buffer, 0, buffer.Length);
            //开始异步接受
            if(!client.ReceiveAsync(receiveAsync))
            {
                //没有挂起手动处理接受
                processsReceive(receiveAsync);
            }

        }
    }

   
    /// <summary>
    /// 处理接受结果
    /// </summary>
    /// <param name="e"></param>
    private void processsReceive(SocketAsyncEventArgs e)
    {
        //请求成功且传输的字节数大于0
        if(e.SocketError==SocketError.Success&&e.BytesTransferred>0)
        {
            //对数据的拷贝,
            byte[] data = new byte[e.BytesTransferred];
            Array.Copy(e.Buffer, e.Offset, data,0, e.BytesTransferred);
            lock (receiveBuffer)
            {
                receiveBuffer.AddRange(data);
            }
            //当缓冲区数据足够时，判断缓存区剩余数据是否足够
            do
            {
                //读取前四个字是包体的长度
                byte[] lengthBytes = receiveBuffer.GetRange(0, 4).ToArray();
                int dataLength = BitConverter.ToInt32(lengthBytes, 0);

                //判断缓存区数据是否 足够完成包体
                if(dataLength<=receiveBuffer.Count-4)
                {
                    byte[] rev = receiveBuffer.GetRange(4, dataLength).ToArray();
                    receiveBuffer.RemoveRange(0, dataLength + 4);

                    //交到前端处理数据
                    DataReceiveEvent?.Invoke(rev);

                }
                else
                {
                    //否则当数据足够大则一直无法终止循环 
                    break;
                }
            } while (receiveBuffer.Count>4);//判断是否有数据且除去开始默认添加的4个

            if(!client.ReceiveAsync(e))
            {
                processsReceive(e);
            }
        }
        else//异常情况
        {
            ProcessError(e);
        }
    }

    public void Send(byte[] data)
    {
        if (data == null || !Connected) return;
        SocketAsyncEventArgs async;
        if(sendAsyncPool.Count>0)
        {
            lock (sendAsyncPool)
            {
                async = sendAsyncPool.Pop();
            }
        }
        else
        {
            //当异步发送对象大于100时停止示例对象
            if (poolCapacity >= 100) return;
            async = new SocketAsyncEventArgs();
            async.RemoteEndPoint = hostEndPoint;
            async.Completed += sendCompleted;
            poolCapacity++;
        }

        //写入 发送数据
        async.SetBuffer(data, 0, data.Length);
        //异步发送，当I/O操作未挂起时调用
        if(!client.SendAsync(async))
        {
            ProcessSend(async);
        }
    }
    /// <summary>
    /// 处理发送结果
    /// </summary>
    /// <param name="e"></param>
    private void ProcessSend(SocketAsyncEventArgs e)
    {
        if (e.SocketError == SocketError.Success)
        {
            if (e != null)
            {
                lock (sendAsyncPool)
                {
                    sendAsyncPool.Push(e);

                }
            }
        }
        else//异常情况
        {
            ProcessError(e);
        }
    }

    private void ProcessError(SocketAsyncEventArgs e)
    {
        //出现异常直接断开服务器
        DisConnectServer();
    }

    bool isConnected;
    /// <summary>
    /// 断开服务器连接
    /// </summary>
    public void DisConnectServer()
    {
        if (isConnected)
        {
            isConnected = false;
            client.Dispose();
            foreach (var asyc in sendAsyncPool)
            {
                asyc.Completed -= sendCompleted;
            }
            sendAsyncPool.Clear();
            poolCapacity = 0;
            receiveAsync.Completed -= ReceiveCompleted;
            //断开服务器委托调用
            DisconnectEvent?.Invoke();
        }
        
    }
    /// <summary>
    /// 发送完成时
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void sendCompleted(object sender, SocketAsyncEventArgs e)
    {
        ProcessSend(e);
    }
    /// <summary>
    /// 接受完成时
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void ReceiveCompleted(object sender, SocketAsyncEventArgs e)
    {
        processsReceive(e);
    }
    /// <summary>
    /// 服务器连接结果
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void connectResult(object sender, SocketAsyncEventArgs e)
    {
        connectResultEvent.Set();//取消阻塞

    }
}
