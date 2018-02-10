% Execution Task Parallel OpenMP Programs in Distributed Memory Environments - Execution Environment
% Johannes Erwerle
% 2018-02-13

# Task-paralleles OpenMP

- normalerweise Parallelisierung von Schleifen
- aber auch Tasks möglich

```
int main(){
	#pragma omp task
	{ //do something }
	#pragma omp task
	{ //do something else }
	#pragma omp task
	{ //do something completly different }
}
```

## Einschränkungen von OpenMP:
- nur 1 Host -> begrenzte Rechenleistung

# Ausführung auf Distributed Memory Systemen

## verwandte Projekte

ompSs

- task-basierte parallele Ausführung
- eigenes API ähnlich wie OpenMP
- erlaubt Ausführung auf distr. Memory Systemen
- funktioniert gut vielen Problemen
- aber nur begrenzt skalierbar:
  - Sämtlicher Speicher muss auf Master-Node passen
  - Speicher wird vom Master-Node ausgeliefert (Memory Bottleneck)
  - Nur Master-Node kann Tasks erzeugen
  - Benchmarks hören bei 16 Nodes auf

# Ausführung auf Distributed Memory Systemen

## Idee
- normales OpenMP C/C++ Code als Eingabe
- Präprozessor modifiziert Code
- linken gegen Code für Execution Environment
- Ausführung über mehrere Nodes verteilt

# Ausführung auf Distributed Memory Systemen

## Präprozessor
- Bachelor-Thesis von Markus Baur
- filtert tasks heraus und speichert diese mit ID ab
- ersetzt `#pragma omp` und ggf. zugeh. Code durch Aufruf and den Runtime-Node
- findet heraus auf welchen Speicher ein Task zugreift

```
int main(){
	create_task(1, //more params e.g. a list of memory addresses)
	create_task(2, ...)
	create_task(3, ...)
}

1 : { //do something }
2 :	{ //do something else }
3 : { //do something completly different }
```

# Ausführung auf Distributed Memory Systemen

## Execution Environment

<!--- - besteht aus Runtime Nodes und Worker Nodes-->
- Runtime Nodes:
  - Verwalten Tasks und Worker
  - koordinieren Synchronisationskonstrukte z.B. `taskwait`
<!---  - Warten ggf. bis Dependencies erfüllt sind 
  - Verteilen Tasks an Worker Nodes
  - koordinieren warten bei `taskwait` und `parallel` Blöcken
  -->
- Worker Nodes
  - Führen die ihnen zugewiesenen Tasks aus
  - Kümmern sich um die Übertragung des zug. Speichers
  <!--- - suspenden Tasks und führen diese auf Anweisung der Runtime Nodes wieder aus -->

# Ausführung auf Distributed Memory Systemen

## Runtime Nodes

- Verwalten jeweils eine feste Menge Worker
- Nehmen neue Tasks entgegen
- Kümmern sich um die Einhaltung von Dependencies
- Verteilen Tasks an Worker in Abhängigkeit von 
- Wecken wartende Tasks wieder auf

# Ausführung auf Distributed Memory Systemen

## Worker Nodes
- Bekommen Taskbeschreibung von Runtime Node zugewiesen
  - task_id, code_id, Liste des benötigten Speichers
- Übertragen Speicher von anderen Workern
- Führen Task als eigene Threads aus
- Schreiben geänderten Speicher zurück
- melden dem Runtime Node wenn ein Task fertig ist

# Ausführung auf Distributed Memory Systemen

## Ausführen von Tasks
- 
