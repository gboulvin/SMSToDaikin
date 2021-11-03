# SMSToDaikin
*Control your Daikin system trough SMS !*

*Controlez votre système Daikin par SMS !*

Le système a été conçu pour controler une pompe à chaleur chaud/froid de marque Daikin.
Il utilise la librairie de danny_source : https://github.com/danny-source/Arduino_DY_IRDaikin qui est compatible avec une grande partie des modèles.
Le module GSM utilisé est un SIM800L
Un module DHT22 a été ajouté afin de pouvoir connaître la température à distance.

Ce projet est utilisé dans une seconde résidence.

Le principe est simple :
Vous envoyez un SMS au module, celui-ci le lit et exécute la commande liée.

Quatre commandes liées au chauffage sont prévues. Envoyez (sans les guillemets) :
* "Chauffage" : Active le mode de chauffe avec un objectif de 18°C, swing ON, ventilateur au maximum (utilisé pour préchauffer la seconde résidence) ;
* "Confort" : Active le mode de chauffe avec un objectif de 24°C, swing OFF et ventilateur lent pour diminuer les nuisances sonores ;
* "Airco" : Active le mode de ventilation seul, swing ON et ventilateur moyen ;
* "Off" : Eteint le système.

Et une commande ("Temp") permet de connaître la température et l'humidité de la pièce.

Le système renvoie systématiquement une confirmation de réception/exécution de la commande

*Une partie du code (lecture du SMS) n'a pas été écrite par moi mais je n'en retrouve plus l'auteur. Merci de me le signaler si vous l'êtes !*
