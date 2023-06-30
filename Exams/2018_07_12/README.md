# Exam 2018/07/12
## English
Modify the ws18.c program so that it can handle multiple HTTP/1.1 requests within the
same TCP connection without using chunks.
Refer to RFC2616.
Try accessing two linked pages with a browser, verifying that the server responds to many
requests without interrupting the connection.
pag1.html:
`<html>Go to <A HREF=”pag2.html”>Page 2</A></html>`
pag2.html:
`<html>Go to <A HREF=”pag1.html”>Page 1</A></html>`
## Italian (original)
Si modifichi il programma ​ws18.c in modo tale che sia in grado di gestire più richieste HTTP/1.1 all’interno della
medesima connessione TCP senza l’uso di chunk.
Si faccia riferimento alla ​RFC2616​.
Si provi con un browser l’accesso a due pagine tra loro linkate verificando che il server risponda a numerose
richieste senza interrompere la connessione.
pag1.html:
`<html>Vai a <A HREF=”pag2.html”>Pagina 2</A></html>`
pag2.html:
`<html>Vai a <A HREF=”pag1.html”>Pagina 1</A></html>`

From https://github.com/nicomazz/ComputerNetworks-unipd2018
