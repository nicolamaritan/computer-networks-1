Si modifichi il programma che implementa il web server in modo che questo, non appena riceve
dal client una request per la risorsa corrispondente al path “/reflect”, anziché cercare un file da
aprire ed inviare, invii al client una response nella quale l’entity body sia
- il testo esatto corrispondente all’intera request inviata dal client al server,
comprensiva di tutti gli elementi che la compongono
- &lt;CRLF&gt;
- L’indirizzo IP in notazione decimale puntata da cui il client ha inviato la propria request
- &lt;CRLF&gt;
- il port da cui il client ha effettuato la propria request
