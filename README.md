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
        - [x] Login
        - [x] Update members
        - [ ] Updates messages
        - [ ] Send
        - [ ] NEXT Remove -> Implements a linked list (better add and remove for the members) then change update members
    - [ ] CLI
        - [ ] Quit function (free)
        - [ ] Send message as server
        - [ ] Disconnect a/all user(s)
        - [ ] Clean messages
