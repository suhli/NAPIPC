const ipc = require("./build/Release/nipc");
const fs = require("fs");
if(!fs.existsSync("tmp")){
    fs.mkdirSync("tmp");
}
const channels = new Set();
module.exports.listen = function (channel,period = 300,cb) {
    channel = `${process.env.PWD}/tmp/${channel}`;
    channels.add(channel);
    if(!fs.existsSync(channel)){
        fs.closeSync(fs.openSync(channel, 'wx'));
    }
    setInterval(()=>{
        let message = ipc.read(channel);
        if(message){
            cb(message);
        }
    },period);
};

module.exports.send = function (channel, msg) {
    channel = `${process.env.PWD}/tmp/${channel}`;
    return ipc.send(channel,msg);
};

process.on("exit",()=>{
    for(let channel of channels){
        fs.unlink(channel);
    }
});

