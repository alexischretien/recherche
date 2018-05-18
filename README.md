# Recherche

## Description

Programme de recherche de documents. 
Le programme recupere les noms de fichiers contenant un certain
nombre d'histoires, place les mots de ces histoires dans des
arbres binaires AVL, ou chaque feuille contient la chaine du 
mot et le nombre d'occurences de ce mot que l'on retrouve dans
le texte associe a cet arbre. Le programme demande de saisir 
des mots clefs pour la recherche et procede au calcul des
metriques pour chaque histoire. Le programme affiche les 
metriques des `NB_LIGNES_RETOUR` histoires qui repondent le mieux
a la requete ainsi que leurs titres et demande de saisir une 
nouvelle requete. Le programme se termine lorsqu'aucun mot 
valide n'est saisie.

## Auteurs

* Bruno Malenfant
* Alexis Chrétien

## Compilation et exécution

```
$ make
$ ./recheche
```
