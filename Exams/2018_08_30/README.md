# Exam 2018/08/30
## English
Modify the wc18.c web client program to handle the control mechanism
caching based on the Last-Modified and If-Modified-Since headers described in RFC2616 at chapters 14.29,
14.25.
Test the program by accessing the root page ( ie / ) of the local web server at port 80 of
88.80.187.84.
The modified wc18.c program must be run twice to verify correct operation. There
first time, with the cache empty, it will have to acquire the date of the last modification of the resource and will save it together with the entity
body to the cache (file on disk).
The second time (and subsequent ones) it will re-download the resource only if it has been modified in the meantime
server, otherwise it will have to access the cached copy.

## Italian (original)
Si modifichi il programma web client wc18.c in modo tale che sia in grado di gestire il meccanismo di controllo del caching basato sugli header Last-Modified e If-Modified-Since descritti nella RFC2616 ai capitoli 14.29, 14.25.
Si faccia il test del programma accedendo alla root page ( cioè / ) del server web locale al port 80 di
88.80.187.84.
Il programma wc18.c modificato, per la verifica del corretto funzionamento, dovrà essere azionato due volte. La
prima volta, a cache vuota, dovrà acquisire la data di ultima modifica della risorsa e la salverà insieme al entity
body sulla cache (file su disco).
La seconda volta (e le successive) ri-scaricherà la risorsa solo se è questa è stata modificata nel frattempo sul
server, altrimenti dovrà accedere alla copia in cache.
