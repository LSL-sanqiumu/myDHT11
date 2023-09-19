/* MQTT Service */
let message;
// 连接选项
const options = {
    // 断开连接后是否清除会话
    clean: true, // true: 清除会话, false: 保留会话
    connectTimeout: 4000, // 超时时间
    reconnectPeriod: 1000, // 重连间隔时间
    // 认证信息
    clientId: 'dht11',	// 客户端ID，最好不要写死，否则起冲突的话就会连不上
    username: 'admin',  // 连接用户名
    password: 'isMQTT20230919', //连接密码，默认为public,新版本登录后台界面会让你修改密码
    
    // 心跳时间，单位为s
    keepalive: 60, 
};
options.clientId = `mqtt_${Math.random().toString(16).slice(3)}`;
console.log('clientId: ' + options.clientId);
// 连接地址, 通过协议指定使用的连接方式
// const connectUrl = 'ws://175.178.181.190:8083/mqtt' //连接服务端地址，注意查看ws协议对应的端口号
const connectUrl = 'wss://ilyd.top:8084/mqtt'; //连接服务端地址，注意查看ws协议对应的端口号
const client = mqtt.connect(connectUrl, options);

client.on('connect', () => {
    console.log('连接成功')
    // 订阅多个主题
    client.subscribe(
        ['test','MyMsg'], // 订阅主题，可以订阅多个
        { qos: 1 },  
        (err) => {
            console.log(err || '订阅成功')
        },
    );
    // 往主题“tourist_enter”发布消息
    client.publish('tourist_enter', 'Topic-tourist_enter:Hello EMQX,我连接上了', 
    (err) => {
        console.log(err || '发布成功')
    })
});
//失败重连
client.on('reconnect', (error) => {
    console.log('正在重连:', error)
});
//连接失败
client.on('error', (error) => {
    console.log('连接失败:', error)
});
//接收消息
client.on('message', (topic, message) => {
    console.log('收到消息：', topic, message.toString());
    var sensor = JSON.parse(message);
    console.log(sensor);
    var dataBox1 = document.getElementsByClassName('dataBox1');
    var dataBox2 = document.getElementsByClassName('dataBox2');
    // console.log(dataBox1);
    dataBox1[0].innerHTML = sensor.temp;
    dataBox2[0].innerHTML = sensor.humi;
});