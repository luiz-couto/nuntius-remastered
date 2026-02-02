# nuntius-remastered
Networked chat room application using ImGui for the user interface and Winsock for network communication

## Protocol Documentation
```
[messageType]
[payload length]
[payload]
    [field A length]
    [field A]
    [field B length]
    [field B]
    ...
```

## Payloads Schemas

### Group Message
```
{   
    messageLength: uint32,
    message: string
}
```

### Private Message
```
{   
    usernameLength,
    username: string,
    messageLength: uint32,
    message: string,
}
```