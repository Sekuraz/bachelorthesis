% Execution of Task Parallel OpenMP Programs in Distributed Memory Environments - Execution Environment
% Johannes Erwerle
% 2018-02-20

# Inhalt

- Überblick OpenMP
- Ausführung auf Distributed Memory System
  - Idee
  - verwandte Projekte
  - Präprozessor
  - Execution Environment
    - Beispiele zur Ausführung von Tasks
  - Aktueller Stand
  - noch fehlende Features und Einschränkungen
- Zusammenfassung

# OpenMP

- API um einfach parallelen Code auf Shared Memory Systemen zu schreiben.
- Umgesetzt durch `#pragma`-Direktiven und div. Funktionen
- klassischerweise Parallelisierung von Schleifen

```
int main() {
	int a[] = new int[1024];

	#pragma omp parallel for
	for(int i = 0; i < 1024; i++){
		a[i] = ...;
	}
}
```

# Task-paralleles OpenMP

- normalerweise Parallelisierung von Schleifen
- aber auch Tasks möglich

```
int main(){
	#pragma omp task
	{ /*do something*/ }
	#pragma omp task
	{ /*do something else*/ }
	#pragma omp task
	{ /*do something completely different*/ }
}
```

## Einschränkungen von OpenMP:
- Benötigt Shared Memory, deswegen nur 1 Node möglich


# Ausführung auf Distributed Memory Systemen

## Idee
- Tasks auf anderen Nodes ausführen

- normaler OpenMP C/C++ Code als Eingabe
- Präprozessor
  - identifiziert Tasks und modifiziert Code
  - baut Execution Environment Code ein
- Execution Environment
  - kümmert sich um Ausführung von Tasks auf anderen Knoten
- Speedup! (hoffentlich)

# Ausführung auf Distributed Memory Systemen

## verwandte Projekte

ompSs

- task-basierte parallele Ausführung
- eigenes API ähnlich wie OpenMP
- erlaubt Ausführung auf Distributed Memory Systemen
- aber nur begrenzt skalierbar:
  - Sämtlicher Speicher muss auf Master-Node passen
  - Nur Master-Node kann Tasks erzeugen
  - Benchmarks hören bei 16 bis 32 Nodes auf

# Ausführung auf Distributed Memory Systemen

## Präprozessor
- Bachelor Arbeit von Markus Baur
- filtert Tasks heraus und speichert diese mit ID ab
- ersetzt Task durch Aufruf für Execution Environment
- findet heraus auf welche Daten ein Task zugreift

# Ausführung auf Distributed Memory Systemen

```
int main(){
	#pragma omp task
	{ /*do something*/ }
	#pragma omp task
	{ /*do something else*/ }
	#pragma omp task
	{ /*do something completely different*/ }
}
```

```
int main(){
	create_task(1, /*more parameters*/);
	create_task(2, ...);
	create_task(3, ...);
}

1 : { /*do something*/ }
2 :	{ /*do something else*/ }
3 : { /*do something completely different*/ }
```

# Ausführung auf Distributed Memory Systemen

## Execution Environment

- Zielarchitektur Distributed Memory Cluster
- Runtime Nodes:
  - Verwalten Tasks und Worker
  - koordinieren Synchronisationskonstrukte z.B. `taskwait`
- Worker Nodes
  - Führen die ihnen zugewiesenen Tasks aus
  - Kümmern sich um die Übertragung des zug. Speichers


# Neue Tasks erzeugen

![create_task](figs/create_task_1.png)

# Neue Tasks erzeugen

![create_task](figs/create_task_2.png)

# Neue Tasks erzeugen

![create_task](figs/create_task_3.png)

# Task starten

![create_task](figs/run_task_1.png)

# Task starten

![create_task](figs/run_task_2.png)

# Task starten

![create_task](figs/run_task_3.png)

# taskwait

![create_task](figs/taskwait_1.png)


# taskwait

![create_task](figs/taskwait_2.png)


# taskwait

![create_task](figs/taskwait_3.png)

# Task beenden

![create_task](figs/finish_task_1.png)

# Task beenden

![create_task](figs/finish_task_2.png)

# Task beenden

![create_task](figs/finish_task_3.png)

# Task beenden

![create_task](figs/finish_task_4.png)

<!--- 

1. Task trifft auf `create_task()`
2. id des erzeugenden Tasks und die code_id des neuen Tasks werden an Runtime Node geschickt
3. neuer Knoten wird im Task-Baum angelegt
4. neue task_id wird in queue eingefügt

# Task ausführen

1. Runtime sucht Worker mit freien Ressourcen
2. Runtime nimmt nächste task_id aus Queue
3. Runtime sucht code_id aus Task-Baum
4. Runtime schickt Nachricht mit (task_id, code_id) an freien Worker
5. Worker führt Tasks aus

# Taskwait

1. Task trifft auf taskwait
2. Sendet Nachricht mit task_id an Runtime Node
3. Runtime setzt waiting Variable im Task-Baum
4. Task wird angehalten
5. Worker führt anderen Task weiter aus oder meldet dem Runtime Node, dass Ressourcen frei sind

# Task beendet

1. Task beendet Ausführung
2. Worker sendet Nachricht mit der task_id des Tasks an Runtime
3. Runtime markiert Task als fertig und propagiert Information an Eltern-Knoten im Task-Baum
4. Falls der Eltern-Knoten wartet und alle Kinder jetzt fertig sind wird dieser aufgeweckt
5. Worker führt anderen Task weiter aus oder meldet dem Runtime Node, dass Ressourcen frei sind

Ähnlich für Warten am Ende von `parallel` Blöcken.
-->

# Aktueller Stand

- Prototyp
  - nicht alle Features von OpenMP
  - Scheduler nur Round Robin
- Umgesetzt als MPI-Programm
- 1 MPI-Prozess pro Node mit mehreren Threads
- 1 Runtime Node mit bel. vielen Workern
- Tasks erzeugen
- mehrere Tasks pro Worker ausführen
- `taskwait` und warten am Ende von `parallel` Blöcken


# Features die noch implementiert werden müssen

- Speichertransfer
  - teuer
- Execution Environment mit Präprozessor verbinden
- Task Dependencies
- Mehrere Runtime-Nodes für Skalierbarkeit

<!--- # Speichertransfer

Speicher wird irgendwann von Task auf einem Knoten allokiert und von diesem
verwaltet.

- Worker bekommt für jeden Tasks Informationen zum Speicher:
  - `node_id`
  - Pointer
  - Länge
- Worker kopiert diesen Speicher von anderem Worker
- führt dann darauf diesen Task aus.
- Zurückschreiben der Änderungen

# Speichertransfer Einschränkungen:

- Hoher Speicher- und Zeitaufwand für Berechnung der Änderungen
- Schlechte Performance, falls Speicher wenig benutzt wird
- schwierig herauszufinden wie groß ein Array ist (Teil des Präprozessors)
- sinnvolles Übertragen von Pointer-Strukturen schwer (Teil des Präprozessors)
  - mehrere Pointer zeigen auf selben Speicher
-->
<!---
# Mehrere Runtime Nodes für Skalierbarkeit

- nur begrenzt viele Worker pro Runtime-Node möglich
- mehrere Runtime Nodes mit jeweils eigenen Workern
- jeden Runtime Node verwaltet seine Worker
- Runtime Nodes senden sich gegenseitig die Länge ihrer Queue
- bei Ungleichgewicht werden Tasks ausgetauscht.
- ausgelagerte Tasks werden zu Dummies im Task-Baum, deren Daten vom anderen Runtime Node
ggf. aktualisiert werden
-->

# Zusammenfassung

- OpenMP Tasks können auf anderen Nodes ausgeführt werden
  - eventuell nicht alle OpenMP Konstrukte sinnvoll umsetzbar
- Vorhandener Code kann hoffentlich beschleunigt werden
- einige Features fehlen noch
- Speicherübertragung ist kritischer Punkt
- viele Möglichkeiten für Performance-Steigerungen
  - Scheduler
  - Shared Memory auf Workern ausnutzen
  - Daten-Lokalität beachten
