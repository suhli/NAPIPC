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

module.exports.listen = function (channel,period = 300,cb) {
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


function rmTmpFile(){
    for(let channel of channels){
        fs.unlinkSync(channel);
    }
    channels.clear();
}

process.on("SIGINT",()=>{
    rmTmpFile();
});

process.on("SIGUSR1",()=>{
    rmTmpFile();
});

process.on("SIGTERM",()=>{
    rmTmpFile();
});
