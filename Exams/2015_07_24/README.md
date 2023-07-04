# Esame 2015/07/24
## English
Modify the Client program to incorporate a caching mechanism for downloaded resources, referring to the following points:
1) Use the HTTP/1.0 Last-Modified header, documented in section 10.10 of RFC 1945.
2) Associate a file to each downloaded resource
  - whose name corresponds to the URL of the resource (in which the character "/" is replaced by the character "_")
  - whose content is composed of
      - a first line containing the download date of the resource (expressed in the mode
cheaper
      - the content of the resource (entity body)
  - whose save directory is ./cache/, child of the program's working directory
proxies.

4) For date management
  - refer to the http-date format (cf. RFC 1945, Section 3.3)
  - use the functions, documented in the UNIX manual in the specific sections listed
in brackets.
    - time(2) to get the date expressed in seconds starting from 1/1/1970 (called
"Epoch" or "Unix time") in the int type renamed time_t
    - localtime(3) to decompose the date expressed in "epoch" into its
components (hours, minutes, ... etc.) each reported in a field of the structure
struct tm and vice versa mktime(3) to perform the reverse operation.
    - optionally use strftime(3) to format (similar to the
printf) the date components present in the fields of the struct tm in one
string and strptime(3) to perform (similar to scanf) the operation
reverse.
5) Proceed as follows:
  - write the flowchart or the pseudo-code on the back of this sheet
corresponding to the logic you plan to use to implement caching on the client
  - the program is modified
  - the website www.example.com is used for testing

## Italian (original)
Si modifichi il programma Client in modo che incorpori un meccanismo di caching delle risorse scaricate, facendo riferimento ai seguenti punti: 
1) Si utilizzi l’header Last-Modified dell’ HTTP/1.0, documentato alla sezione 10.10 della RFC 1945.
2) Ad ogni risorsa scaricata si associ un file
  - il cui nome corrisponde all’URL della risorsa (nel quale il carattere “/” viene sostituito dal carattere “_”)
  -  il cui contenuto è composto da
      - una prima riga contenente la data di download della risorsa (espressa nel modo
più conveniente
      - il contenuto della risorsa (entity body)
  - la cui cartella di salvataggio è ./cache/, figlia del working directory del programma
proxy.

4) Per la gestione della data
  - si faccia riferimento al formato http-date (cfr. RFC 1945, Sezione 3.3)
  - si utilizzino le funzioni, documentate nel manuale UNIX nelle apposite sezioni riportate
tra parentesi.
    - time(2) per ottenere la data espressa in secondi a partire dal 1/1/1970 (detta
“Epoch” o “Unix time”) nel tipo int rinominato time_t
    - localtime(3) per scomporre la data espressa in “epoch” nelle sue
componenti (ora, minuti, ... etc.) riportate ciascuna in un campo della struttura
struct tm e viceversa mktime(3) per effettuare l’operazione inversa.
    - opzionalmente utilizzare strftime(3) per formattare (analogamente alla
printf) le componenti della data presenti nei campi della struct tm in una
stringa e strptime(3) per effettuare (similmente alla scanf) l’operazione
inversa.
5) Si proceda come segue:
  - si scriva sul retro del presente foglio il diagramma di flusso o il pseudo-codice
corrispondente alla logica che si intende utilizzare per implementare la cache sul client
  - si modifichi il programma
  - si utilizzi per le prove il sito web www.example.com
