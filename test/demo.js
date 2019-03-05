const ipc = require("../index");
const channel = `pingpong`;
const cluster = require("cluster");
if(cluster.isMaster){
    const ping_cluster = cluster.fork();
    const pong_cluster = cluster.fork();

    ping_cluster.on("online",()=>{
        function ping(){
            ipc.sendAsync(`${channel}_ping`,"ping😁",(status,result)=>{
                // console.log(`async status:${status},result:${result}`);
            });
           // ipc.send(`${channel}_ping`,"ping");
        }
        ipc.listen(`${channel}_pong`,(msg)=>{
            console.log(`ping receive:${msg}`);
            ping();
        },300);
        ping();
    });

    pong_cluster.on("online",()=>{
        function pong(){
            ipc.send(`${channel}_pong`,"pong🤣");
        }
        ipc.listen(`${channel}_ping`,(msg)=>{
            console.log(`pong receive:${msg}`);
            pong();
        },300);
    });
}

