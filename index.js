const ipc = require("./build/Release/nipc");
const fs = require("fs");
if(!fs.existsSync("tmp")){
    fs.mkdirSync("tmp");
}
const channels = new Set();
const existPaths = new Set();
function initPath(path){
    if(existPaths.has(path)){
        return;
    }
    if(!fs.existsSync(path)){
        fs.closeSync(fs.openSync(path, 'wx'));
        existPaths.add(path);
    }
}

module.exports.listen = function (channel,cb,period = 300) {
    channel = `${process.env.PWD}/tmp/${channel}`;
    channels.add(channel);
    initPath(channel);
    setInterval(()=>{
        let message = ipc.read(channel);
        if(message){
            cb(message);
        }
    },period);
};

module.exports.send = function (channel, msg) {
    channel = `${process.env.PWD}/tmp/${channel}`;
    initPath(channel);
    return ipc.send(channel,msg);
};

module.exports.sendAsync = function (channel, msg,callback) {
    channel = `${process.env.PWD}/tmp/${channel}`;
    initPath(channel);
    return ipc.send_async(channel,msg,callback);
};

module.exports.clearChannel = function (channel) {
    channel = `${process.env.PWD}/tmp/${channel}`;
    return ipc.clear_channel(channel);
};

function rmTmpFile(){
    for(let channel of channels){
        fs.unlinkSync(channel);
    }
    channels.clear();
}

process.on("SIGINT",()=>{
    process.nextTick(rmTmpFile)
});

process.on("SIGUSR1",()=>{
    process.nextTick(rmTmpFile)
});

process.on("SIGTERM",()=>{
    process.nextTick(rmTmpFile)
});
