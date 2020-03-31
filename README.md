# col3-client-etu

Squelette du programme client Clash of L3

# Installation

## clonage du dépôt 

Pour cloner le dépôt depuis un terminal
```
git clone https://manouchian.univ-smb.fr/syncproc/col3-client-etu.git

```
## Construction du projet

### Pré-requit

la bibliothèque col3-bib a besoin de NCURSE
```
sudo apt install libncurses-dev
```

### Construction manuelle
C'est un projet 'autotools', donc il suffit d'aller dans le dossier projet et d'utiliser les commandes habituelles:
```
autoreconf -i
./configure
make
```
### Construction avec Anjuta

1. Lancez Anjuta
1. Cliquez sur 'Import a projet'
1. Cliquez sur 'Import from folder' puis sélectionnez le dossier racine du projet
1. Cliquez sur 'Import'
1. Sélectionnez le moteur 'Base Autotools'
1. Une fois le projet importé, cliquez sur 'Build project' puis 'Execute'

