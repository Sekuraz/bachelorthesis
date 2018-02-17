# Execution of Task Parallel OpenMP Programms in Distributed Memory Environments - Execution Environment

OpenMP bietet neben der Möglichkeit Schleifen zu parallelisieren unter anderem auch eine Unterstützung für die parallele Ausführung von Tasks.
Da OpenMP ein Shared Memory System benötigt, ist die zur Verfügung stehende Rechenleistung begrenzt.
In diesem Vortrag geht es um eine Ausführungsumgebung mit der Task paralle OpenMP Programme auf Distributed Memory Clustern ausgeführt werden können.
Hierzu wird der Programmcode von einem Präprozessor analysiert und modifizert (Bachelor-Arbeit von Markus Baur).
Die Ausführungsumgebung verteilt dann zur Laufzeit die Tasks auf mehrere Nodes, sorgt für den nötigen Datenaustausch und die Verwaltung von Synchronisations-Konstrukten.
