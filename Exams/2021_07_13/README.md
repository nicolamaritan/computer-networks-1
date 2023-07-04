# Exam 2021/07/13
## English
Modify the web proxy program reported in the exam.c file in your folder
staff to behave as follows
1) the web proxy, as soon as it receives a request for a resource from the web client, will perform its own
times many requests to the web server, each of which will download a segment of the entity
body of the requested resource of length equal to 1000 bytes until the entire entity body
will be downloaded (the last segment will obviously have a length ≤ 1000 bytes).
2) The web proxy will send the entire entity body of the resource to the web client via a single response,
thus reporting in a single stream all the segments downloaded from the server in the correct order, yes
that for the web client the segmented download will be completely transparent.
In order to implement the function, refer to the header Range of the defined HTTP/1.1
in RFC 2616: sections 14.35, 3.12, 14.16.

## Italian (original)
Si modifichi il programma del web proxy riportato nel file esame.c presente nella vostra cartella
personale in modo tale che si comporti come segue
1) il web proxy, non appena riceverà dal web client una request di una risorsa, effettuerà a sua
volta molte request al web server, ciascuna delle quali scaricherà un segmento dell’entity
body della risorsa richiesto di lunghezza pari a 1000 bytes fino a che l’intero l’entity body
risulterà scaricato (l’ultimo segmento avrà ovviamente una lunghezza ≤ 1000 bytes).
2) Il web proxy invierà l’intero entity body della risorsa al web client tramite un’unica response,
riportando così in un unico stream tutti i segmenti scaricati dal server nell’ordine corretto, sì
che per il web client lo scaricamento a segmenti risulterà completamente trasparente.
Al fine di implementare la funzione, si faccia riferimento all’header Range dell’HTTP/1.1 definito
nella RFC 2616: sezioni 14.35, 3.12, 14.16.
