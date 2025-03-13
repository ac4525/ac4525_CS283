1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

The remote client can determine when a command's output is fully received by either checking the message length or when the server is disconnected. To handle partial reads, the client can repeatedly call recv() until the entire message is received. Using a length prefix or delimiter ensures complete message transmission by telling when the client to stop reading.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

A networked shell protocol can mark the start and end of a command by using a special character or by sending the length of the command. Without this, commands could get displayed incorrectly or cause errors because TCP does not automatically separate messages.

3. Describe the general differences between stateful and stateless protocols.

Stateful protocols keeps track of past interactions and uses that information for future messages. Stateless protocols treat each message independently without storing previous interaction data.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

Although UDP is unreliable, it is faster because it does not have error handling. Overall, we would use UDP for speed by sacrificing reliability.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

The interface/abstraction provided by the operating system to enable applications to use network communications is socket interface.