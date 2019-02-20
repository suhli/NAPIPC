
node IPC through shared memory written with N-API demo

install:
```bash
npm install
```

build:
```bash
    npm run conf && cd build && make
```
    
run:
```bash
    node test/sender.js
    #sending!
    #send success,result:e hello world!
    node test/listener.js
    #listening channel:test1
    #receive:e hello world!
```