Università degli Studi di Padova
Dipartimento di Ingegneria dell’Informazione
Esame di Reti di Calcolatori - 12 Luglio 2018
Prof. ing. Nicola Zingirian

Si modifichi il programma ​ws18.c in modo tale che sia in grado di gestire più richieste HTTP/1.1 all’interno della
medesima connessione TCP senza l’uso di chunk.
Si faccia riferimento alla ​RFC2616​.
Si provi con un browser l’accesso a due pagine tra loro linkate verificando che il server risponda a numerose
richieste senza interrompere la connessione.
pag1.html:
<html>Vai a <A HREF=”pag2.html”>Pagina 2</A></html>
pag2.html:
<html>Vai a <A HREF=”pag1.html”>Pagina 1</A></html>

From https://github.com/nicomazz/ComputerNetworks-unipd2018
