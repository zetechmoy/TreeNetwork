<<<<<<< HEAD
Workflow git:

https://fr.atlassian.com/git/tutorials/comparing-workflows/gitflow-workflow

https://danielkummer.github.io/git-flow-cheatsheet/index.fr_FR.html
=======
# Projet Réseau G5

An implementation of a network using trees, based on CTOP [https://cedric.cnam.fr/fichiers/RC209.pdf](URL)

## Installation

```
git clone git@gitlab.insa-cvl.fr:projet-reseau-3a/groupe-5.git groupe-5
cd groupe-5
```
### Install in folder

```
make clean
make
```

### Install in global

```
make clean
make install
```

## Use


```
./app {internet_interface} {name}
```
Example

```
./app wlp1s0 insa
```

Then use the /help command to get informations about what to do
```
Wait for action or enter command :
/help
**/attach [PARENT_PORT] [PARENT_IP] **
    => attach current process to a group by its ip/port
**/attna [PARENT_NAME] **
	=> attach current process to a group by its name /!\ NOT IMPLEMENTED YET
**/detach [GROUP_PORT] **
	=> detach current process from another (parent or child) by its port
**/detna [GROUP_NAME] **
	=> detach current process from another (parent or child) by its name
**/msg [GROUP_PORT] [GROUP_IP] "MSG" **
	=> send a message to a group using ip/port of the group, all children of this group will receive this message
**/msgna [GROUP_NAME] "MSG" **
	=> send a message to a group using name of the group, all children of this group will receive this message
**/info**
	=> show current process name, ip and port
**/children **
	=> show children of current process
**/parents **
	=> show parents of current process
**/routes **
	=> show routes (children and children of children and ...) of current process
**/help **
    => show this help message
**/exit **
    => stop current process, children will attach to all of current parents

```

## Authors

* Théo Guidoux
* Clément Foissard
* Clément Charpentier
* Pierre Vinois
>>>>>>> release
