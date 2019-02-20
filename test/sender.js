const ipc = require("../build/Release/nipc");
const msg = "e hello world!";
const channel = "test1";
try {
    console.log("sending!");
    let result = ipc.send(channel,msg);
    console.log(`send success,result:${result}\n`);
}catch (e) {
    console.log(`sender exception\n${e}`);
    process.exit(1);
}