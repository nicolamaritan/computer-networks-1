# Exam 2020/01/23
## English
Modify the Web Server program developed during the course in such a way that it forbids, via Error
Code 403 (cf. RFC2616, chap. 10.4 ), to a conventional user agent (browser) to access the
resource /file2.html if the same user agent has not previously accessed it
to the resource /file1.html.
To implement this function, the HTTP state management mechanisms, called
Cookies, referring to the examples in section 3.1 of RFC6265 and the grammars in
Section 4 of the same document.
Cookies represent a variant of the authentication mechanism seen in class. In fact al
instead of requiring the client to send a WWW-authenticate header reporting in each request
username and password, requires the client to send a header to the server in each request showing a
string (the cookie) communicated by the server to the client at the first access. This allows the server to
understand that many requests come from the same user agent, without the need to use one
authentication,
N.B.
1. it should not be sufficient for the user agent to access /file1.html once and for all for
then having permission to access /file2.html many times. Conversely, after giving the
permission to access /file2.html, the server will forbid a second access if first the
same user agent will not have accessed /file1.html again.
2. the user agent will not be forced to access /file1.html just before accessing the
/file2.html. On the contrary, the server must also be able to allow the access sequence
/file1.html, ...<other resources>..., /file2.html.

## Italian (original)
Si modifichi il programma Web Server sviluppato durante il corso in modo tale che vieti, tramite Error
Code 403 (cfr. RFC2616, cap. 10.4 ), ad un user agent (browser) convenzionale di accedere alla
risorsa /file2.html se prima il medesimo user agent non abbia precedentemente avuto accesso
alla risorsa /file1.html.
Per implementare questa funzione si utilizzino i meccanismi di gestione dello stato HTTP, detti
Cookies, facendo riferimento agli esempi della sezione 3.1 della RFC6265 e alle grammatiche nella
Sezione 4 del medesimo documento.
I Cookies rappresentano una variante del meccanismo di autenticazione visto a lezione. Infatti al
posto di richiedere al client di inviare in ogni request un header WWW-authenticate riportante
username e password, richiede al client di inviare al server in ogni request un header riportante una
stringa (il cookie) comunicata dal server al client al primo accesso. Questo permette al server di
capire che tante richieste provengono da uno stesso user agent, senza necessità di utilizzare una
autenticazione,
N.B.
1. non dovrà essere sufficiente allo user agent accedere una volta per tutte a /file1.html per
poi aver il permesso di accedere tante volte al /file2.html. Al contrario, dopo aver dato il
permesso di accedere al /file2.html, il server vieterà un secondo accesso se prima il
medesimo user agent non avrà avuto accesso di nuovo al /file1.html.
2. lo user agent non sarà obbligato ad accedere al /file1.html subito prima di accedere al
/file2.html. Al contrario il server deve poter permettere anche la sequenza di accesso
/file1.html, ...<altre risorse>..., /file2.html.
