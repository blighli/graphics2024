

public class DataModel 
{
    /// <summary>
    /// 消息类型
    /// </summary>
    public byte Type { get; set; }
    /// <summary>
    /// 请求类型
    /// </summary>
    public byte Reequest { get; set; }
    /// <summary>
    /// 消息体
    /// </summary>
    public byte[] Message { get; set; }
    
    public DataModel(byte type,byte request,byte[]message)
    {
        Type = type;
        Reequest = request;
        Message = message;
    }

    public DataModel()
    {

    }
}
