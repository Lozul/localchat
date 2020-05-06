# A local chat server

> This is a local chat server write in C and Javascript. While the chat is hosted on a computer using a wifi network, every other device connected to the same network should be able to join the chat room by using the IP of the hoster.

## Tree
```
.
├── www
│   ├── css
│   │   └── style.css
│   ├── js
│   │   └── script.js
│   ├── rsc
│   │   └── ...
│   ├── chat.html
│   └── login.html
├── Makefile
├── README.md
└── server.c
```

## TODO
- [x] HTML
- [x] Javascript (client side)
- [x] CSS
- [ ] C (server side)
    - [x] Server socket
    - [ ] Commands
        - [ ] Login -> TODO: implements user list + add when login
        - [ ] Update
        - [ ] Send
        - [ ] Remove
    - [ ] CLI
        - [ ] Quit function (free)
        - [ ] Send message as server
        - [ ] Disconnect a/all user(s)
        - [ ] Clean messages
