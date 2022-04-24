# EE450 Socket Programming Project

------
- Name: Yaxing Cai
- USC ID: 9552812062

## Compilation

Please make sure your compiler supports `C++ 11` or higher.

```bash
git clone https://github.com/cyx-6/EE450Project.git
cd EE450Project
make all
```

## Executable Target

Please execute the target in following order:

1. `./serverM`
2. `./serverA`
3. `./serverB`
4. `./serverC`
5. `./clientA` or `./clientB`

The order of 4 servers is restricted with above order, or it will fail to accept requests from clients. However, since the method `poll` has been applied, the order of `./clientA` and `./clientB` makes no difference.

## Supported Commands

| Name | Format | Description |
| --- | --- | --- |
| `CHECK WALLET` | `./clientA <username1>` | check the current balance of `username1` |
| `TXCOINS` | `./clientA <username1> <username2> <transfer amount>` | transer `<transfer amount>` from `username1` to `username2` |
| `TXLIST` | `./clientA TXLIST` | export all the transactions |
| `stats` | `./clientA <username1> stats` | list the user-based statistical summary of `<username1>` |

## Files

| Name | Description |
| --- | --- |
| client.h | header file for `Client` class |
| server.h | header file for `Server` class |
| backend.h | header file for `Backend` class |
| clientA.cpp | implementation file for clientA |
| clientB.cpp | implementation file for clientB |
| serverM.cpp | implementation file for main server |
| serverA.cpp | implementation file for backend serverA |
| serverB.cpp | implementation file for backend serverB |
| serverC.cpp | implementation file for backend serverC |
| user.h | header file for `User` class |
| operation.h | header file for `Operation` class |
| transaction.h | header file for `Transaction` class |
| utils.h | header file for utility network helper functions |
| config.h | header file for global configuration |

## Message Format

| Class | Format |
| --- | --- |
| `int`, `string`| None |
| `Operation` | `type\|userName1\|userName2\|transferAmount\|serialID\|` |
| `Transaction` | `serialID\|userName1\|userName2\|transferAmount\|` |
| `User` | `ranking\|userName\|transactionNumber\|balance\|initialBalanceAdded\|` |

And the specific types are listed below:

| Name | Type |
| --- | --- |
| `type` | `enum` |
| `userName`, `userName1`, `userName2` | `string` |
| `serialID`, `transferAmount`, `ranking`, `transactionNumber`, `balance` | `int` |
| `initialBalanceAdded` | `bool` |

## Protocol

### CHECK WALLET

The main server forwards the `CHECK WALLET` operation from client to backend servers, merges the user information from all backend servers, and then replies to the client.

| Sender | Receiver | Format | Content |
| --- | --- | --- | --- |
| clientA or clientB | serverM | `Operation` | `CHECK WALLET` |
| serverM | serverA, serverB and serverC | `Operation` | `CHECK WALLET` |
| serverA, serverB and serverC | serverM | `User` | user information |
| serverM | clientA or clientB | `User` | user information |

### TXCOINS

The main server decomposes the `TXCOINS` operation to 2 `CHECK WALLET` operations for both users first, and requests the backend servers over both user information. After analyzing the replies from backend servers, the main server decides whether the original `TXCOINS` operation is randomly forwarded to one backend server, and then reples to the client.

| Sender | Receiver | Format | Content |
| --- | --- | --- | --- |
| clientA or clientB | serverM | `Operation` | `TXCOINS` |
| serverM | serverA, serverB and serverC | `Operation` | `CHECK WALLET` |
| serverA, serverB and serverC | serverM | `User` | user information |
| serverM | serverA, serverB or serverC | `Operation` | `TXCOINS` |
| serverM | clientA or clientB | `User`, `User` | transaction result |

### TXLIST
The main server forward the `TXLIST` operation from client to backend servers, merges the transaction logs from all backend servers, and then exports it to `alicoins.txt`.

| Sender | Receiver | Format | Content |
| --- | --- | --- | --- |
| clientA or clientB | serverM | `Operation` | `TXLIST` |
| serverM | serverA, serverB and serverC | `Operation` | `TXLIST` |
| serverA, serverB and serverC | serverM | `list<Transaction>` | transaction logs |

### stats
The main server forward the `stats` operation from client to backend servers, merges the user information from all backend servers, and then exports it to `alicoins.txt`.

| Sender | Receiver | Format | Content |
| --- | --- | --- | --- |
| clientA or clientB | serverM | `Operation` | `stats` |
| serverM | serverA, serverB and serverC | `Operation` | `stats` |
| serverA, serverB and serverC | serverM | `list<User>` | user information |
| serverM | clientA or clientB | `list<User>` | user information |

### Details

1. Initially, each backend server will send the local max `serialID` to the main server as greeting message, and help the main server determine the next available `serialID` as well.
2. The main server communicates with only one client and one backend at any time. Once the main server gets the replies from one backend server, it will send request to the next server then.
2. For the messages in `list` format, the senders always send the size of the `list` first, then start sending each entry of the `list`.
3.  For TCP connecions, there is always an acknowledgement from the receiver in response. It usually contains the information it has just received.

## Reference
The implementation refers to the basic socket functions and general connection framework in [Beej's Guide to Network Programming][1] initially, and their detailed APIs in [Linux man pages online][2]. But no reused code beyond the system built-in socket functions, like `connect`, `accept`, `bind`, etc.

[1]: https://beej.us/guide/bgnet/html/
[2]: https://man7.org/linux/man-pages/index.html