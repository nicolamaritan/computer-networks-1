# Exam 2022/07/01
## English
The HTML page test.html that you find in your /home/<number> calls several files
of images (1.png, 2.png,...,9.png) also present in the "home". The client, interpreting the
HTML text in test.html, automatically make all the necessary requests to compose the
page by finding, one by one, all 9 images and incorporating them into the page.
The <matricola>.c program (which is the Web Server developed in class) after you will have some
The custom port will allow your local PC browser to access the URL
http://<hostname>:<port>/test.html with all 9 embedded images, receiving the related requests
and sending the consequent responses (transactions) according to RFC2616,
- In sequence
- One for each connection.
Apply the minimum modifications to the program <serial number>.c such that the server
- Supports multiple transactions (request/response) within the same connection
- Supports transactions (request/response) on multiple concurrent connections
The <serial number>.c program must output the sequence of requests made by indicating
filename, referenced socket, and transaction counter per connection, for example:


/test.html Socket 4 trans 0

/1.png Socket 4 trans 1

/7.png Socket 6 trans 0

/9.png Socket 5 trans 0

/4.png Socket 4 trans 2

/5.png Socket 5 trans 1

...

## Italian (original)
La pagina HTML prova.html che trovate nella vostra /home/<matricola> richiama diversi file
di immagini (1.png, 2.png,...,9.png) anch’essi presenti nella “home”. Il client, interpretando il
testo HTML in prova.html, effettua automaticamente tutte le request necessarie comporre la
pagina reperendo, ad una ad una, tutte le 9 immagini ed incorporandole nella pagina.
Il programma <matricola>.c (che è il Web Server sviluppato a lezione) dopo che ne avrete
personalizzato il port consentirà al browser del vostro PC locale di accedere all’URL
http://<hostname>:<port>/prova.html con tutte le 9 immagini incorporate, ricevendo le relative request
ed inviando le conseguenti response (transazioni) secondo la RFC2616,
- In sequenza
- Una per ogni connessione.
Si applichino al programma <matricola>.c le minime modifiche tali che il server
- Supporti più transazioni (request/response) nell’ambito della stessa connessione
- Supporti transazioni (request/response) su più connessioni concorrenti
Il programma <matricola>.c deve riportare in output la sequenza di richieste effettuate indicando
nome file, socket di riferimento e contatore delle transazione per connessione, ad esempio:


/prova.html Socket 4 trans 0

/1.png Socket 4 trans 1

/7.png Socket 6 trans 0

/9.png Socket 5 trans 0

/4.png Socket 4 trans 2

/5.png Socket 5 trans 1

...
