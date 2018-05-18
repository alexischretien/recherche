# Recherche

## Description

Programme de recherche de documents. 
Le programme récupere les noms de fichiers contenant un certain
nombre d'histoires, place les mots de ces histoires dans des
arbres binaires AVL où chaque feuille contient la chaine du 
mot et le nombre d'occurences de ce mot dans le texte associé. 
Le programme demande de saisir des mots clefs pour la recherche 
et procède au calcul des métriques pour chaque histoire. 
Le programme affiche les métriques des `NB_LIGNES_RETOUR` 
histoires qui répondent le mieuxà la requête ainsi que leurs 
titres et demande de saisir une nouvelle requête. Le programme 
se termine lorsqu'aucun mot valide n'est saisie.

## Auteurs

* Bruno Malenfant
* Alexis Chrétien

## Compilation et exécution

```
$ make
$ ./recheche
```
