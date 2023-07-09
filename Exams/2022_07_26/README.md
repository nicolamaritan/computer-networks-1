# Exam 2022/07/26
## English
The openoffice.org website allows, in addition to secure access (HTTPS on port 443), also
non-secure access (HTTP on port 80) and provides for the manual selection of the content language,
via a selector located on the home page at the top right, which calls up the specific URL
corresponding to the home page written in the selected language.
Given these assumptions, it is required to modify the program <serial number>.c (which is the source
of the Web Proxy developed in class), present in your `/home/<number>` so that
- Use your custom port, to which the Microsoft Edge browser can connect for
access mode via proxy (properly configured using the menu
edge://settings/?search=proxy, under "Other Computer Proxy Settings").
- When the user accesses the openoffice.org home page with the browser configured for
the preferred language depending on how it is set via the menu
edge://settings/languages, the proxy will deliver the contents of the
home page corresponding to the preferred language, without the user having to use
manually the selector on the web page. The languages ​​that the proxy will be able to select will be
only Italian, German, French, Spanish, according to the priorities expressed by the
browser, or English if the browser will express preferences for still other languages, or for English
same.
NB: Refer to RFC 2616, Sections 14.4 and 14.12, for language management in HTTP.

## Italian (original)
Il sito web openoffice.org consente, oltre all’accesso sicuro (HTTPS su port 443), anche
l’accesso non sicuro (HTTP su port 80) e prevede la selezione manuale della lingua dei contenuti,
tramite un selettore posto nella home page in alto a destra, che richiama l’URL specifico
corrispondente alla home page scritta nella lingua selezionata.
Dati questi presupposti, si richiede di modificare il programma <matricola>.c (che è il sorgente
del Web Proxy sviluppato a lezione), presente nella vostra `/home/<matricola>` in modo tale che
- Utilizzi il vostro port personalizzato, al quale il browser Microsoft Edge potrà collegarsi per la
modalità di accesso tramite proxy (opportunamente configurato agendo sul menù
edge://settings/?search=proxy, alla voce “Altre impostazioni Proxy del Computer”).
- Quando l’utente accederà alla home page di openoffice.org con il browser configurato per
la lingua preferita a seconda di come viene impostata tramite il menù
edge://settings/languages, il proxy provvederà a recapitare al client i contenuti della
home page corrispondente alla lingua preferita, senza che l’utente debba utilizzare
manualmente il selettore sulla pagina web. Le lingue che il proxy potrà selezionare saranno
solamente italiano, tedesco, francese, spagnolo, a seconda delle priorità espresse dal
browser, o inglese se il browser esprimerà preferenze per altre lingue ancora, o per l'inglese
stesso.
NB: Si faccia riferimento alla RFC 2616, Sezioni 14.4 e 14.12, per la gestione della lingua in HTTP.
