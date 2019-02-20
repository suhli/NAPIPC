const ipc = require("../build/Release/nipc");
const channel = "test1";
console.log("listening channel:" + channel);
while(true){
    try{
        let message = ipc.read(channel);
        if(message){
            console.log("receive:" + message);
        }
    }catch (e) {
        console.log(`reader exception\n${e}`);
        process.exit(1);
    }
}
