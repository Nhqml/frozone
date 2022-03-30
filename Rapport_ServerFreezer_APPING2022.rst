========================================================================================================================
Server Freezer: Un demon Linux pour cartographier et geler la configuration d’un OS
========================================================================================================================


:Auteurs: Michel San, Styvell Pidoux, Erfan Hormozizadeh, Valentin Seux, Kenji Gaillac, Théophane Wallerich


.. image:: ../img/LSE-logo.png
        :scale: 500


.. contents::



.. section-numbering::




Préambule
==========

Ce document contient les principaux éléments relatifs à notre projet de fin d’étude dans le cadre de notre formation en apprentissage à l’EPITA.
Il contient une brève présentation du sujet ainsi qu’un état de l’art scindé en plusieurs parties. Il contient également les spécifications techniques, la documentation de l’outil développé et enfin une brève conclusion décrivant l’avancement, la maturité du projet et les difficultés rencontrées.

Introduction
============


L’objectif du projet FREEZER est de développer un démon configurable de cartographie d’une machine (Serveur, mais également PC utilisateur, serveur embarqué type raspberry) qui permette de geler la configuration de cette machine, c’est-à-dire d’empêcher la création de nouvelles actions (processus, connexion réseau, etc) tout en conservant le comportement existant. Il s'agit en fait de créer une whitelist de ressources, fichiers, connexions autorisées sur la machine et de restreindre son utilisation a cette seule whitelist.

 Ce mécanisme va se découper en deux parties distinctes :

- La cartographie : Permet de lister tous les éléments importants d’une configuration d’une machine spécifique. Il s’agit en quelque sorte de prendre une photo du système d’exploitation à un instant T. Cela permet notamment de définir le fonctionnement standard, légitime d’une machine afin de détecter des divergences par rapport à celle-ci.

- Le Freezer : Permet de Geler une configuration précise, c’est-à-dire empêcher toutes divergence par rapport à celle-ci en bloquant tout création ou accès des ressources.

====

Objectifs de la solution
========================

Les applications et les objectifs sont multiples étant donne qu'il s'agit d'une lib en deux modules, chaque module pouvant avoir divers usages.

Monitorer le système
++++++++++++++++++++



La génération d'un fichier d'information simple et concis permet un d'avoir un aperçu complet de l'activité sur la machine. Nous ne traitons pas ici le monitoring de performance ou d'état de sante de la machine mais seulement de l'activité sur celle-ci. On pourrait par exemple automatiser la génération du fichier a intervalles réguliers et gérer son historisation afin de garder des logs clairs de l'activité sur la machine. On pourrait également générer un Dashboard et /ou un système d’alerting basé sur les logs générés.


Mandatory Access Control: Bloquer, même le user Root
++++++++++++++++++++++++++++++++++++++++++++++++++++

La plupart des OS mainstream sont base sur le modèle DAC (Discretionary Access Control). Cela permet de définir notamment des droits sur des fichiers, un utilisateur possédant un fichier est autorisé à écrire et à modifier les permissions de celui-ci. Cependant il s'agit d'un modèle dit discrétionnaire, c'est à dire qu'il confère le pouvoir a l'utilisateur root qui possède tous les droits, il n'est pas contraint pas la politique de contrôle d'accès. Cela peut notamment poser problème lors de la compromission d'un système si l'attaquant dispose d'un accès root directement ou s’il a la possibilité d'élever ses privilèges.

Il existe un autre modèle, qui viens seulement en tant que surcouche de l'OS que l'on appel MAC (Mandatory Access Control) qui permet de renforcer la politique de sécurité. Les contrôle d'accès y sont obligatoires, même l'utilisateur root ne peut les contourner. Une fois que la politique est en place, les utilisateurs ne peuvent pas la modifier même s’ils ont les privilèges root. Les protections sont indépendantes des propriétaires.


Evidemment il y a toujours un moyen de Bypass cette solution pour qui voudrait vraiment le faire, mais cela force l'attaquant a réévaluer sa méthode d'attaque, l'accès root n'étant pas synonyme de plus haut niveau de privilège il s'agit d'un utilisateur comme les autres.

A l'origine le renforcement des politiques de contrôle d'accès a été largement démocratise par le projet SELinux conçu par la NSA et confie à la communauté open source en 2000.


Mitiger une attaque
+++++++++++++++++++

Notre module kernel peut egallement permetre de bloquer des ressources specifique independemment, ce qui peut permettre de mitiger une attaque en temps reel. En bloquant toutes les connexions de la machine par exemple.

Geler une infrastructure
++++++++++++++++++++++++

Cela sert notamment à s'assurer qu'une machine ou une infrastructure de machine suit uniquement un comportement défini. Le développement d'un outil simple et léger se révèle être très intéressant s’il peut s'appliquer du hardware simple tel qu'un rapsberry pi ou de l'iot en général. En particulier car la sécurité est faible dans ce genre d'environnement. Un freeze des connections de matériel iot en général permettrais d'éviter l'utilisation de ce materiel dans des attaques DDOS dites amplifiées par exemple.

====

====


Etat de l’Art
=================

Ce projet de Démon Linux de cartographie système est un projet intimement lié aux systèmes d’EDR/XDR/IDS et de Monitoring de système. Il est également très similaires a des features propose par certains patchs du noyau linux pour le renforcement de la sécurité.

Dans un premier temps, la partie Cartographie est largement couverte par un ensemble de solutions open sources testées et approuvées depuis un certain nombre d’années.

La partie Freezer quant à elle, reste plus "inexplorée". Il peut s'agir soit d'un patch de sécurité supplémentaire du noyau linux ou soit d'un système d’EDR(Endpoint Detection & Response) XDR (Extended Detection & Response).

Solutions de cartographies
++++++++++++++++++++++++++

Tout d'abord la cartographie, il s'agit d'obtenir une vue globale d'un OS, l'état global du système a un instant T.
Comme explicité précédemment la cartographie des systèmes Linux est une méthode bien maitrisées et éprouvée depuis des années.

On pense tout de suite aux outils de monitoring comme outils de cartographie, ils peuvent être local, de façon à obtenir un aperçu de sa propre machine, ou peuvent fonctionner avec un serveur central permettant d'obtenir une vue global d'un ensemble de machine.
Néanmoins nous cherchons ici à pouvoir définir ensuite un modèle base sur la cartographie réalisée, et nous voulons un outil simple et léger, la plupart des solutions de monitoring sont surtout axee performance et peuvent être lourde à mettre en place.
Le patch linux GR security propose une feature de génération automatique d'ACL (Learning mode) qui est en fait une cartographie des ressources.



Monitoring
##########

Distribué
---------

Zabbix [https://github.com/zabbix/zabbix](https://github.com/zabbix/zabbix)

C'est une solution de Monitoring open source qui va permettre également une récupération d’informations de systèmes linux, pour créer des Dashboards et superviser une infrastructure technique, cette solution est cependant plus orientée alerting.


Local
-----

Il existe également d’autres solutions de monitoring systèmes plus legère fonctionnant en local sur la machine.


Linux Dash (Graphical web interface) : https://github.com/afaqurk/linux-dash

IDS
###

On peut citer également des solutions de monitoring fonctionnant pour les operating systemes Linux.
Nous pouvons également citer l’ensemble des IDR (Incident Detection System) et EDR .(Endpoint Detection & Response). Des mécanismes de cyberdéfense apparu plus récemment dans l’histoire.

Ces deux mécanismes intègrent des solutions de détection de menaces dit ‘Anomaly based’ qui vont donc nécessiter un monitoring précis du système protégé et donc une cartographie de celui-ci. Nous effectuons bien la distinction avec les systèmes Signature Based qui ne nécessitent pas de cartographier le système et nous nous concentrons ici sur les IDS dit Host Based. (HIDS)


Lib
###

[https://github.com/jmigot-tehtris/psutil]
Un outil écrit en Python, il existe un équivalent Rust et c'est une librairie extrêmes complète et facile à utiliser qui couvre tous les besoins de cartographie incluant même les performances et les metrics hardware.

Extension de Kernel
###################

Le module GR Security une extension pour le kernel linux qui en augmente sa sécurité, présente une feature de cartographie et de gel comme nous le verrons dans la partie suivante. Il s’agit d’un patch à appliquer au kernel et qui va lui apporter des features supplémentaires, notamment les Mandatory control list. GR Security possède une feature très intéressante qu’ils appellent le Learning Mode et qui permet en analysant l’activité sur une machine de définir une ACL précise et restrictive.

GR Security : [https://github.com/linux-scraping/linux-grsecurity]

Nous pourrions continuer cette liste avec une multitude de solutions utilisant le même concept de cartographie système. Il est relativement facile de trouver des solutions open sources pour ce type d’analyse, nous nous contenterons donc de l’open source pour la partie cartographie.

Tableau
#######

+-----------------------------+--------------------------------+--------------------+-------------------------+------------------+------------------------------+-----------------------------+------------------+-----------------------------------------+----------------------------+
| Name                        | Type                           | Lang               | OPEN/COMMERCIAL         | get  users       | get  processes               | get connections             | get  files       | Others                                  | OpenBsd  Compatible        |
+=============================+================================+====================+=========================+==================+==============================+=============================+==================+=========================================+============================+
| psutil Python               | lib                            | Python             | OPEN                    | V                | V                            | V                           | V                | Performance + hardware metrics          | V                          |
+-----------------------------+--------------------------------+--------------------+-------------------------+------------------+------------------------------+-----------------------------+------------------+-----------------------------------------+----------------------------+
| psutil Rust                 | lib                            | Rust               | OPEN                    | V                | V                            | V                           | V                | Performance + hardware metrics          | V                          |
+-----------------------------+--------------------------------+--------------------+-------------------------+------------------+------------------------------+-----------------------------+------------------+-----------------------------------------+----------------------------+
| px                          | lib                            | Python             | OPEN                    | V                | V                            | V                           | V                | Performance +hardware metrics           | V                          |
+-----------------------------+--------------------------------+--------------------+-------------------------+------------------+------------------------------+-----------------------------+------------------+-----------------------------------------+----------------------------+
| libstatgrab                 | lib                            | C                  | OPEN                    | V                | V                            | F                           | F                | Performance metrics, filesystem, mutex  | V                          |
+-----------------------------+--------------------------------+--------------------+-------------------------+------------------+------------------------------+-----------------------------+------------------+-----------------------------------------+----------------------------+
| Linux Dash                  | UI Dashboard                   | MULTI (JS)         | OPEN                    | V                | V                            | V                           | V                | Performances metrics                    | F                          |
+-----------------------------+--------------------------------+--------------------+-------------------------+------------------+------------------------------+-----------------------------+------------------+-----------------------------------------+----------------------------+
| Nagios                      | Supervision distribuée         | C                  | OPEN                    | V                | V                            | V                           | F                | Performances metrics                    | V                          |
+-----------------------------+--------------------------------+--------------------+-------------------------+------------------+------------------------------+-----------------------------+------------------+-----------------------------------------+----------------------------+
| GR Security(Learning mode)  | Linux extension, Kernel Patch  | C                  | OPEN                    | V                | V                            | V                           | V                | Automated ACL generation                | F                          |
+-----------------------------+--------------------------------+--------------------+-------------------------+------------------+------------------------------+-----------------------------+------------------+-----------------------------------------+----------------------------+
| what_file                   | Utility                        | Python             | OPEN                    | F                | V                            | F                           | V                |                                         | V                          |
+-----------------------------+--------------------------------+--------------------+-------------------------+------------------+------------------------------+-----------------------------+------------------+-----------------------------------------+----------------------------+
| Interact Software           | Supervision distribuée         | C++/C#             | COM                     | V                | V                            | V                           | F                |                                         | F(Windows)                 |
+-----------------------------+--------------------------------+--------------------+-------------------------+------------------+------------------------------+-----------------------------+------------------+-----------------------------------------+----------------------------+




Solutions de Gel de Configurations
++++++++++++++++++++++++++++++++++

La fonction de Freeze est-elle moins explorée, c’est principalement une feature des EDR/XDR, qui permet de contenir une menace lorsque celle-ci est détéctée sur une des machines surveillées. Une « réaction immunitaire”.
Il peut egallement s'agir des politiques d'ACL plus pousse permise par des patch du kernel (module kernel).




EDR/XDR
########

On peut citer tout d'abord l'outil commercial Crowstrike, et son falcon agent sensor deployable sur un grand nombre d'OS. C'est l'un des leaders actuels en matière d'EDR et de défense active. Il permet de features de gel, ou de contention qui permette de bloquer des ressources.

Pour citer un exemple français, l’Open XDR Plateform regroupe un ensemble de solution de cyber sécurité française, pour couvrir l’ensemble des problématiques pour le entreprises, le but étant de concurrencer les géants du secteur. Parmi ses solutions, l’XDR Harfang lab contient un outil de remédiation qui permet d’isoler des machines précises, c’est-à-dire bloquer des connexions réseaux ainsi que d’empêcher la création de nouveaux processus précis. Cette solution est recommandée par l'ANSSI. La solution Thetris est également française (Bordeaux).

L’étude des features de ses solutions est relativement compliqués, les documentations techniques précises sont relativement rares, majoritairement remplacées par des documents publicitaires/marketing sans réel valeurs scientifiques. Lorsque l’information n’est pas disponible publiquement nous choisirons le symbole ? dans le tableau suivant

ACL(Access Control List)
#############################

C'est une gestion plus poussée des contrôles d'accès que propose le module kernel gr-security ou encore RSBAC. La génération de ces whitelist peut être laisse a l'administrateur, ou génèré (appris) automatiquement pour gr-security


Tableau
#######

+-----------------------------------+-----------------------+-------------------+----------------------+-----------------------+--------------------+--------------------------------+---------------------+------------------------+------------------------------------+
| Name                              | Type                  | Lang              | OPEN/COMMERCIAL      | Block Users           | Block Proc         | Block    Connexion             | Block Files         | Freeze ALL             | OpenBsd Compatible                 |
+===================================+=======================+===================+======================+=======================+====================+================================+=====================+========================+====================================+
| Crowstrike                        | EDR                   | ?                 | COM                  | ?                     | V                  | V                              | ?                   | F                      | V                                  |
+-----------------------------------+-----------------------+-------------------+----------------------+-----------------------+--------------------+--------------------------------+---------------------+------------------------+------------------------------------+
| Darktrace                         | EDR                   | ?                 | COM                  | ?                     | V                  | V                              | ?                   | ?                      | ?                                  |
+-----------------------------------+-----------------------+-------------------+----------------------+-----------------------+--------------------+--------------------------------+---------------------+------------------------+------------------------------------+
| GR-Security                       | Kernel patch          | C                 | OPEN                 | V                     | V                  | V                              | V                   | V                      | F                                  |
+-----------------------------------+-----------------------+-------------------+----------------------+-----------------------+--------------------+--------------------------------+---------------------+------------------------+------------------------------------+
| RSBAC                             | Kernel patch          | C                 | OPEN                 | V                     | V                  | V                              | V                   | V                      | F                                  |
+-----------------------------------+-----------------------+-------------------+----------------------+-----------------------+--------------------+--------------------------------+---------------------+------------------------+------------------------------------+
| Thetris                           | XDR                   | ?                 | COM                  | ?                     | ?                  | ?                              | ?                   | ?                      | F                                  |
+-----------------------------------+-----------------------+-------------------+----------------------+-----------------------+--------------------+--------------------------------+---------------------+------------------------+------------------------------------+
| Harfang Lab                       | XDR                   | ?                 | COM                  | ?                     | V                  | V                              | ?                   | ?                      | ?                                  |
+-----------------------------------+-----------------------+-------------------+----------------------+-----------------------+--------------------+--------------------------------+---------------------+------------------------+------------------------------------+







Analyse techniques des éléments de la Cartographie système
==========================================================

La cartographie du système va se résumer à la collectes d’informations, on demande au système de nous renvoyer un certain nombre d’informations que l’on va structurer de sorte à obtenir un aperçu complet du système. Cette partie va se résumer dans un premier temps à la création de 3 fonctions C au sein de notre librairie.

Liste des ressources à cartographier
++++++++++++++++++++++++++++++++++++


Utilisateurs: get_users
########################

L’idée ici va être de récupérer la liste des utilisateurs de la machine, connectés ou non.

Commande Linux : w

Arbre de Processus: get_processes
##################################

Concernant les processus actifs sur la machine, il est indispensable d’obtenir un arbre structuré contenant l’ensemble des processus lancés et leur provenenance.

Commande Linux : top

Ports locaux ouvert/ Connexion distantes: get_connections
#########################################################

Il est primordial de connaitre précisément l’ensemble des points d’accès à une machine, c’est-à-dire la liste des port locaux ouvert, et les connexions actives à une machine ainsi que les protocoles utilisés.

Commande Linux : netstat

Listes des fichiers accédés: get_files
######################################

Ia liste des fichiers ouverts ainsi que leurs proprietés (proprietaires, droits…) Va permettre de completer l’overview du Système.

Schema de la solution
++++++++++++++++++++++

.. image:: ../img/Carto.png
        :scale: 300

Impact sur le système d’exploitation
++++++++++++++++++++++++++++++++++++

Cette partie est extrêmes légère en terme de charge pour le Système d’exploitation car elle n’utilise aucune surcharge particulière et s’occupe uniquement de consulter des informations via des fichiers/mécanismes Linux prévus pour cela. Nous considèrerons comme **négligeable** l’impact de notre module de Cartographie sur le Système d’Exploitation

Analyse technique des solutions de Gel de configuration
===========================================================

Cette Partie va décrire les solutions techniques mises en place afin de permettre un Gel de la configuration de la Machine. Elle va être basé sur un principe que l’on appelle Hooking d’appel système pour avoir le maximum de contrôle sur le système d’exploitation hôte. Pour permettre en particulier le blocage de l’utilisateur root.


Liste des ressources à geler
++++++++++++++++++++++++++++

Lock User
#########

Lock Process
############

Lock Write
##########

Lock Connection
###############

Unlock
#######

Pour avoir une solution tout à fait fonctionnelle elle doit permettre de déverrouiller/dégeler une ressource, afin de ne pas paralyser le système.

.. warning::

	A noter que cette fonction doit être sécurisée si l’on veut définir une vraie politique de Mandatory Access control, le but est de compliquer la tâche pour l’attaquant même si celui-ci dispose des privilèges root il ne doit pas pouvoir unlock lui-même les ressources.


Comparaison de solutions de blocage
++++++++++++++++++++++++++++++++++++


+---------------------+-------------+-------------+--------------+-----------+
| Nom                 | Scope       | Simplicité  | Portabilité  | Contrôle  |
+=====================+=============+=============+==============+===========+
| Wrapper Shell       | Userland    | OUI         | OUI          | NON       |
+---------------------+-------------+-------------+--------------+-----------+
| /etc/ld.so.preload  | Userland    | OUI         | OUI          | !         |
+---------------------+-------------+-------------+--------------+-----------+
| Module kernel       | Kernelland  | !           | !            | OUI       |
+---------------------+-------------+-------------+--------------+-----------+


Hooking d’appel système
+++++++++++++++++++++++

Le hooking ou « Contournement » d’appel système va permettre un placement strategique au sein du système d’exploitation. Les syscall faisant le lien entre Userland et KernelLand, détourner/controller ceux-ci permet un contrôle total sur les fonctions vitales du système. Cela va donc nous permettre de bloquer différents mécanismes de façon certaine. Meme l'utilisateur root sera contraint par ce blocage.




Schéma de la solution retenue
+++++++++++++++++++++++++++++

.. image:: ../img/hook.png
	 :scale: 400

Exemple pour le blocage de connexion:
######################################

.. image:: ../img/hook1.png
	 :scale: 400

Impact sur le système d’exploitation
++++++++++++++++++++++++++++++++++++

L’impact sur le Système d’exploitation va cette fois-ci être non négligeable puisque l’on va surcharger chaque appel système. Cela va consister dans les fait a un parcours de tableau de taille maximum 1024?? a chaque appel système hooké.





Documentation Frozone
==================================

Module Carto
++++++++++++

Module Blocker
++++++++++++++

Portage OpenBSD
+++++++++++++++

freezer/Makefile : lib/modules/build

carto/src/utils.h : <error.h> not found --> include <err.h> , error() --> err()

carto/include/carto.h : <utmpx.h> not found --> include <utmp.h> , struct utmpx --> struct utmp

QA
==

.. image:: ../img/hook2.png
	 :scale: 400

Projet
==========

Organisation
+++++++++++++

- Michel San: gestion de la pipeline Gitlab + Vagrant / Dev Freezer


- Styvell Pidoux: Dev Freezer


- Kenji Gaillac: Partie Cartographie


- Valentin Seux: Dev Cartographie

- Erfan Hormozizadeh: Portage OpenBsd

-Theophane Wallerich: Gestion de projet, redaction rapport / Test de performances

Les developpeurs se chargent d'ecrire les test unitaires/fonctionnelles

Etat d’avancement
+++++++++++++++++

Le projet contient a l'heure actuelle.

Fonctionnelle sous Ubuntu 20.04:

Une api de 4 fonctions permettant de generer un fichier contenant la cartographie du systeme

Un module kernel contenenant des fonctions permeattant de bloquer les syscall relatifs aux ressources (users,process,files,connections) et de debloquer les ressources fonctionnant avec une whiteliste permmettant d'authoriser des utilisations de syscall pour certains utilisateur ou process.

A completer


Difficultés rencontrées
+++++++++++++++++++++++

- Utilisation de C pour la partie Userland

- Portage sous OpenBsd du module Kernel



Poster Ing
++++++++++

Remerciements
+++++++++++++

Licence
========


Références
================


Man Linux

https://www.linux.com/news/securing-linux-mandatory-access-controls/

https://www.kernel.org/

[https://syscalls64.paolostivanin.com/]


[http://www.ouah.org/LKM_HACKING.html%23I.1]

Cyber Imunnity: A bio inspired Cyber defence System [https://link.springer.com/chapter/10.1007/978-3-319-56154-7_19]

