# Exam 2015/07/03
## English
Modify the program that implements the web server so that this as soon as it receives
from the client a request for the resource corresponding to the path "/reflect", instead of looking for a file from
open and send, you send the client a response in which the entity body is
- the exact text corresponding to the entire request sent by the client to the server,
including all the elements that compose it
- &lt;CRLF&gt;
- The IP address in dotted decimal notation from which the client sent its request
- &lt;CRLF&gt;
- the port from which the client made its request

## Italian (original)
Si modifichi il programma che implementa il web server in modo che questo, non appena riceve
dal client una request per la risorsa corrispondente al path “/reflect”, anziché cercare un file da
aprire ed inviare, invii al client una response nella quale l’entity body sia
- il testo esatto corrispondente all’intera request inviata dal client al server,
comprensiva di tutti gli elementi che la compongono
- &lt;CRLF&gt;
- L’indirizzo IP in notazione decimale puntata da cui il client ha inviato la propria request
- &lt;CRLF&gt;
- il port da cui il client ha effettuato la propria request
