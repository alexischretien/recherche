/**
 * UQAM - Hiver 2017 - INF3105 - Groupe 10 - TP2
 *
 * inf3105.cpp - Fichier source c++ d'un programme de recherche de document. 
 *               Le programme recupere les noms de fichiers contenant un certain
 *               nombre d'histoires, place les mots de ces histoires dans des
 *               arbres binaires AVL, ou chaque feuille contient la chaine du 
 *               mot et le nombre d'occurences de ce mot que l'on retrouve dans
 *               le texte associe a cet arbre. Le programme demande de saisir 
 *               des mots clefs pour la recherche et procede au calcul des
 *               metriques pour chaque histoire. Le programme affiche les 
 *               metriques des NB_LIGNES_RETOUR histoires qui repondent le mieux
 *               a la requete ainsi que leurs titres et demande de saisir une 
 *               nouvelle requete. Le programme se termine lorsqu'aucun mot 
 *               valide n'est saisie. 
 *
 * @Auteurs      Alexis Chretien (CHRA25049209)
 *               Bruno Malenfant
 * @Version      31 mars 2017
 */

#include "DocumentXML.h"
#include "Histoire.h"
#include "arbreavl.h"

#include <list>
#include <string>
#include <utility>
#include <math.h>
#include <iomanip>
#include <iostream>

using namespace std;

/**
 * Constantes
 */
const int     NB_LIGNES_RETOUR = 5;
const string  MESSAGE_REQUETE  = "requete : ";
const string  MESSAGE_FIN      = "Aucune mot valide saisi. Fin de l'execution.";


/**
 * Classe Mot - Classe d'un objet a inserer dans les arbres AVL. 
 *
 * @membre  mot     La chaine de caractere formant un mot contenu dans une
 *                  histoire.
 * @membre  nombre  Si l'objet est insere dans une arbre associe a une histoire, 
 *                  represente le nombre d'occurence de ce mot dans l'histoire.
 *                  Si l'objet est insere dans l'arbre "nbHistoiresContientMot", 
 *                  ce nombre represente le nombre d'histoires contenant ce
 *                  mot.
 */
class Mot {
    public:
        string mot;
        int nombre;
       
        Mot (const Mot& unMot) 
          : mot(unMot.mot), nombre(unMot.nombre) {
        }

        Mot (const string& unMot, const int& unNombre) 
          : mot(unMot), nombre(unNombre) {
        }

        bool operator==(const Mot& unMot) const {
           return mot == unMot.mot;
        }
        bool operator<(const Mot& unMot) const {
            return mot < unMot.mot;
        }      
       
        friend ostream& operator<<(ostream& a, const Mot& unMot) {
            //ostream& a;
            a << "( \"" << unMot.mot << "\" , " << unMot.nombre << " )";
            return a;
        }
}; 

/**
 * Fonction qui prend une chaine de caracteres et qui renvoie la chaine 
 * equivalente, en minuscules.
 *
 * @param   mot     Le mot a traiter.
 * @return          Le mot de retour, ou tous les caracteres alphabetiques sont 
 *                  en minuscules.
 */
string enMinuscules(string const &mot) {

    string motRetour;
    
    for (int i = 0 ; i < mot.length() ; ++i) {
        motRetour += tolower(mot[i]);
    }
    return motRetour;
}

/**
 * Fonction qui traite une chaine de caracteres entree par l'utilisateur.
 *
 * La fonction enleve tous les caracteres non-alphabetiques en debut et en fin
 * de la chaine. si, au milieu d'un mot, un caractere autre qu'une lettre ou le
 * caractere tiret '-' est present, ce caractere est considere comme un caractere
 * separateur. Deux caracteres tirets successifs ("--") sont consideres comme un
 * caractere separateur. 
 *
 * @param  chaine       La chaine de caracteres a traiter.
 * @return              Un vector<string> contenant des mots de recherche valides.
 */
vector<string> traiterChaine(string const &chaine) {
    
    int debutMot;
    int nbCarac;
    bool trouverFinMot;
    string chaineMin;
    vector<string> mots;
 
    chaineMin = enMinuscules(chaine);

    for (int i = 0 ; i < chaineMin.length() ; ++i) {
        
        // trouver le premier caractere d'un mot, doit etre une lettre
        if (chaineMin[i] >= 'a' && chaineMin[i] <= 'z') {
            
            debutMot = i;    
            nbCarac = 1;
            trouverFinMot = false;

            // parcourir le reste de la chaine pour trouver la fin du mot
            while(++i < chaineMin.length() && !trouverFinMot) {                
               
                // si un charactere tiret, verifier qu'il soit suivit d'au moins
                // au autre caractere alphabetique
                if (chaineMin[i] == '-') {
                    if (i < chaineMin.length() - 1 
                            && chaineMin[i + 1] >= 'a' 
                            && chaineMin[i + 1] <= 'z') {   

                        ++nbCarac;
                    }
                    else {
                        trouverFinMot = true;
                    }
                }
                // si caractere alphabetique, continuer
                else if ((chaineMin[i] >= 'a' && chaineMin[i] <= 'z')) {
                        ++nbCarac;
                } 
                // tout autre caractere, on a trouve la fin du mot
                else {
                    trouverFinMot = true;
                }
            }
            mots.push_back(chaineMin.substr(debutMot, nbCarac));
            trouverFinMot = true;
            --i; 
        }      
    }
    return mots;
}
 
/**
 * Fonction qui appelle la fonction "lireDocuments" et renvoie un pointeur
 * vers un vecteur de pointeur d'histoires permettant d'iterer sur les
 * mots des differentes histoires ainsi que d'en extraire les titres. 
 *
 * @param  a_nomFichier   Le nom du fichier XML a lire. 
 * @return                Un pointeur sur un vector<Histoire*>.
 *
 * @Auteur Bruno Malenfant.
 */
vector< Histoire *> * lireDocuments( string a_nomFichier ) {

    vector<Histoire *> * histoires = new vector< Histoire * >();
    DocumentXML * listeFichiers = lireFichierXML( a_nomFichier );

    Element * courrant = listeFichiers->racine();
    vector< Contenu * >::const_iterator it = courrant->begin();

    // trouver <liste>
    while( ! ( * it )->estElement() ) ++ it;
    courrant = ( Element * )( * it );

    for( Contenu * contenu : * courrant ) {
        if( contenu->estElement() ) {
            Element * element = ( Element * )contenu;

            DocumentXML * doc = lireFichierXML( element->attribut( string( "fichier" ) ) );

            vector<Histoire *> * h = extraireHistoires( * doc );

            histoires->insert( histoires->end(), h->begin(), h->end() );
        }
    }
    return histoires;
}

/**
 * Fonction qui initialise les arbres AVL des differentes histoires. 
 *
 * Un arbre AVL est instancie pour chaque histoire listee dans "listeDocument.xml".
 * Chaque feuille des arbres correspond a un mot different, et contient la frequence
 * de ce mot dans l'histoire en question (son nombre d'occurences).
 *
 * @param  arbresHistoires  Reference d'un pointeur d'ArbreAVL<Mot>. A la fin de la 
 *                          fonction, pointe sur le premier arbreAVL<Mot> instancie.
 * @param  titresHistoires  Reference d'un pointeur de string. A la fin de la fonction,
 *                          pointe sur le titre de l'histoire associe au premier 
 *                          ArbreAVL<Mot> instancie. 
 * @return                  Le nombre d'histoires traitees. 
 */
int recupererDonnees(ArbreAVL<Mot>*& arbresHistoires, 
                     string*& titresHistoires) {


    int nbHistoires;
    vector< string >::const_iterator iterHistoire;
    vector< string >::const_iterator iterHistoireFin;
    Mot* unMot;

    vector<Histoire *> * histoires = lireDocuments( string ("listeDocument.xml") );
    nbHistoires = histoires->size();
     
    arbresHistoires = new ArbreAVL<Mot>[nbHistoires];
    titresHistoires = new string[nbHistoires];

    int i =0;
    // Pour chaque histoires
    for (Histoire * histoire : * histoires) {
        
        titresHistoires[i] = histoire->titre();
        ArbreAVL<Mot>::Iterateur iterMot(arbresHistoires[i]);
            
        iterHistoire    = histoire->begin();
        iterHistoireFin = histoire->end();
            
        // parcourir les mots de l'histoires
        for ( ; iterHistoire != iterHistoireFin ; ++iterHistoire) {

            unMot = new Mot (enMinuscules(*iterHistoire), 1);
            iterMot = arbresHistoires[i].rechercher(*unMot);            

            // Si le mot n'est pas dans l'arbre de l'histoire, ajouter le "Mot". 
            if(!iterMot) {
                arbresHistoires[i].inserer(*unMot);    
            }  
            // si le mot est deja dans l'arbre de l'histoire, incrementer le nombre d'occurences.
            else {
                unMot->nombre = (*iterMot).nombre + 1;
                arbresHistoires[i].inserer(*unMot);
            } 
        }
        ++i;
    }
    return nbHistoires;
}

/**
 * Fonction qui instancie un arbre AVL contenant tous les mots contenus dans
 * les histoires. 
 *
 * Chaque feuille associee a un mot contient le nombre d'histoires qui contiennent
 * ce mot. 
 *
 * @param  nbHistoires      Le nombre d'histoires.
 * @param  arbreRetour      Reference de l'arbre AVL a remplir. 
 * @param  arbresHistoires  les arbres AVL associes a chacune des histoires. 
 * @return
 */
void calculerNbDocumentsContientMot(const int& nbHistoires, 
                                   ArbreAVL<Mot>& arbreRetour, 
                                   ArbreAVL<Mot>* const & arbresHistoires) {
    int i; 
    Mot* unMot;

    // Pour chaque histoire.
    for (i = 0 ; i < nbHistoires ; ++i) {
        ArbreAVL<Mot>::Iterateur iterMot    = arbresHistoires[i].debut();
        ArbreAVL<Mot>::Iterateur iterMotFin = arbresHistoires[i].fin();
        
        // Parcourir les feuille de arbresHistoires[i] 
        for ( ; iterMot != iterMotFin ; ++iterMot) {
   
            unMot = new Mot((*iterMot).mot, (*iterMot).nombre);
            ArbreAVL<Mot>::Iterateur iterArbreRetour = arbreRetour.rechercher(*unMot);
  
            // Si l'arbre de retour ne contient pas un mot contenu dans arbresHistoires[i],
            // ajouter ce mot a l'arbre de retour. 
            if (!iterArbreRetour) {
                unMot = new Mot ((*iterMot).mot, 1);
                arbreRetour.inserer(*unMot);
            }
            // Si l'arbre de retour contient ce mot, incrementer la valeur du nombre de 
            // documents contenant ce mot.
            else {
                unMot = new Mot ((*iterArbreRetour).mot, (*iterArbreRetour).nombre + 1);
                arbreRetour.inserer(*unMot);
            }
        }
    }
}

/**
 * Fonction qui calcule la frequence de document inverse pour un mot donnee.
 *
 * @param   nbHistoires            Le nombre total d'histoires.
 * @param   nbHistoiresContientMot Le nombre d'histoires qui contiennent le mot.
 */
double frequenceDocumentInverse(const int& nbHistoires, const int& nbOccurences) {

    return log2 (nbHistoires /(double) nbOccurences);
}

/**
 * Fonction qui calcule la metrique de pertinence des differentes histoire pour
 * une requete donnees. 
 *
 * @param  arbresHistoires              Les arbres AVL des differentes histoires, 
 *                                      permet de recuperer le nombre d'occurences
 *                                      d'un mot dans chaque histoire.  
 * @param  arbreNbHistoiresContientMot  L'arbre AVL permettant de recuperer le
 *                                      nombre d'histoires qui contiennent un mot. 
 * @param  motsRequetes                 Un vecteur de chaines de caracteres, ou 
 *                                      chaque chaine correspond a un mot valide
 *                                      de la requete de recherche. 
 * @param  nbHistoires                  Le nombre d'histoires total. 
 * @return                              Un pointeur de doubles correspondant aux 
 *                                      metriques de pertinence de chaque histoire.
 */
double* calculerMetrique(ArbreAVL<Mot>* const &arbresHistoires, ArbreAVL<Mot> const &arbreNbHistoiresContientMot,
                         vector<string> const &motsRequete, const int& nbHistoires) {

    int nbDocumentsContientMot;
    double* metriques;
    Mot* unMot;

    metriques = new double[nbHistoires];

    // pour chaque histoire
    for (int i = 0 ; i < nbHistoires ; ++i) {
        metriques[i] = 0.0;

        // pour chaque mot de la requete
        for (string mot : motsRequete) {
            unMot = new Mot(mot, 1);
            ArbreAVL<Mot>::Iterateur iter = arbresHistoires[i].rechercher(*unMot);

            if (iter) {
                nbDocumentsContientMot = (*arbreNbHistoiresContientMot.rechercher(*unMot)).nombre;
                metriques[i] += (frequenceDocumentInverse(nbHistoires, nbDocumentsContientMot) * (*iter).nombre); 
            }
            delete unMot;
        }    
    }
    return metriques;
}

/**
 * Fonction qui trie et affiche les titres des NB_LIGNES_RETOUR histoires 
 * repondant le mieux a une requete de recherche ainsi que leur metrique, en
 * ordre decroissant de pertinence. 
 *
 * La fonction utilise le tri-monceau pour effectuer le tri. La deuxieme etape 
 * n'est effectue que sur NB_LIGNES_RETOUR elements (les meilleurs resultats
 * seront quand meme en fin de tableau).
 *
 * S'il y a moins de NB_LIGNES_RETOUR histoires, le programme affiche les 
 * metriques et les titres de toutes les histoires, aussi en ordre decroissant de 
 * pertinence.  
 *
 * @param  metriques        Les valeurs des metriques de pertinence pour une requete
 *                          associees a chaque arbre. 
 * @param  titresHistoires  Le titres des differentes histoires.
 * @param  nbHistoires      Le nombre d'histoires. 
 * @return
 */
void trierEtAfficherCinqMeilleurs(double* const &metriques, string* const &titresHistoires, int const &nbHistoires) {

    int posParent;
    int posEnfant;
    int posEnfant2;
    int posEnfantMax;
    int diff_hist_ligneRetour;
    bool finiTamiser;
    pair < string, double >* enfantMax; 
    pair < string, double > resultats[nbHistoires + 1]; 
 
    diff_hist_ligneRetour = nbHistoires - NB_LIGNES_RETOUR;

    // Pour empecher segmentation fault si les fichiers xml contiennent moins d'histoires 
    // que le nombre de lignes de retour souhaitees.
    if (diff_hist_ligneRetour < 0) {
        diff_hist_ligneRetour = 0;
    }

    //tri-monceau, premiere partie
    for (int i = 1 ; i <= nbHistoires ; ++i) {
        resultats[i] = make_pair(titresHistoires[i-1], metriques[i-1]);
        posParent = i/2; 
        posEnfant = i;
        while (posParent > 0  && resultats[posEnfant].second > resultats[posParent].second) {
            resultats[posEnfant].swap(resultats[posParent]);
            posEnfant = posParent;
            posParent = posEnfant/2; 
        }
    }   

    //tri-monceau, deuxieme partie (partielle)
    for (int i = nbHistoires ; i > diff_hist_ligneRetour ; --i) {
        finiTamiser = false;
        resultats[1].swap(resultats[i]);
        posParent  = 1;
        posEnfant  = 2;
        posEnfant2 = 3;
        while (posEnfant < i && !finiTamiser) {
          
            //si l'enfant 1 et l'enfant 2 existe
            if (posEnfant2 < i) {

                // trouver le maximum des 2 enfants
                if(resultats[posEnfant].second > resultats[posEnfant2].second) {
                   enfantMax = &resultats[posEnfant];
                   posEnfantMax = posEnfant; 
                }
                else {
                    enfantMax = &resultats[posEnfant2];
                    posEnfantMax = posEnfant2;
                }                
            }
            // si seulement enfant 1 existe, enfant1 est l'enfant maximum
            else {
               enfantMax = &resultats[posEnfant];
               posEnfantMax = posEnfant;
            }
            // si le parent est plus petit que l'enfant maximum, echanger
            if (resultats[posParent].second < enfantMax->second) {
                resultats[posParent].swap(*enfantMax);
                posParent = posEnfantMax;
                posEnfant = posParent*2;   
                posEnfant2 = posEnfant+1;
            }
            else {
                finiTamiser = true;
            }
        }
    }

    // affichage
    cout << showpoint;
    for (int i = nbHistoires ; i > diff_hist_ligneRetour  ; --i) {
        cout << resultats[i].second << " : " << resultats[i].first << endl;
    }  
   cout << endl; 
}

/**
 * Main
 */
int main(int argc, char** argv) {

    int nbHistoires;
    double* metriques;
    string requete;
    string* titresHistoires;
    vector<string> motsRequete;
    ArbreAVL<Mot>* arbresHistoires;
    ArbreAVL<Mot>  arbreNbHistoiresContientMot;

    nbHistoires = recupererDonnees ( arbresHistoires, titresHistoires );
    calculerNbDocumentsContientMot ( nbHistoires, arbreNbHistoiresContientMot, arbresHistoires );

    do {
        cout << MESSAGE_REQUETE;
        getline(cin, requete); 
        motsRequete = traiterChaine( requete );

        if (motsRequete.size() > 0) {
            metriques = calculerMetrique( arbresHistoires, arbreNbHistoiresContientMot, 
                                          motsRequete, nbHistoires );
            trierEtAfficherCinqMeilleurs( metriques, titresHistoires, nbHistoires );
        }
    }
    while ( motsRequete.size() > 0 );
    
    cout << MESSAGE_FIN << endl;
	return 0;
}
