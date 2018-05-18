/*  INF3105 | Structures de données et algorithmes
 *  UQAM | Département d'informatique | Automne 2016
 *  Arbre AVL pour Lab6 + TP2
 *  http://ericbeaudry.ca/INF3105/
 */
#if !defined(__ARBREAVL__H__)
#define __ARBREAVL__H__
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "pile.h"

template <class T>
class ArbreAVL 
{
  public:
    //Constructeurs et destructeur.
    ArbreAVL();
    ArbreAVL(const ArbreAVL&);
    ~ArbreAVL();

    //Fonction pour manipuler l'arbre.
    bool contient(const T&) const;
    int  hauteur() const;
    void inserer(const T&);
    bool vide() const;
    void vider();
    void enlever(const T&); // non requis pour le TP2.

    // Annonce l'existance d'une classe Iterateur.
    class Iterateur;
    
    // Fonction pour obtenir un itérateur (position dans l'arbre)    
    Iterateur debut() const;
    Iterateur fin() const;
    Iterateur rechercher(const T&) const;
    Iterateur rechercherEgalOuSuivant(const T&) const;
    Iterateur rechercherEgalOuPrecedent(const T&) const;

    // Accès aux éléments de l'arbre via un intérateur.
    const T& operator[](const Iterateur&) const;
    T& operator[](const Iterateur&);
    
    // Copie d'un arbre AVL.
    ArbreAVL& operator = (const ArbreAVL&);

  private:
    struct Noeud{
        Noeud(const T&);
        T contenu;
        int equilibre;
        Noeud *gauche;
        Noeud *droite;
    };
    Noeud* racine;

    // Fonctions internes.
    bool inserer(Noeud*&, const T&);
    void rotationGaucheDroite(Noeud*&);
    void rotationDroiteGauche(Noeud*&);
    void vider(Noeud*&);
    void copier(const Noeud*, Noeud*&) const;
    const T& max(Noeud*) const;
    bool enlever(Noeud*&, const T& e);

  public:
    // Sera présenté à la semaine #7
    class Iterateur{
      public:
        Iterateur(const ArbreAVL& a);
        Iterateur(const Iterateur& a);
        Iterateur(const ArbreAVL& a, Noeud* c);

        operator bool() const;
        bool operator!() const;
        bool operator==(const Iterateur&) const;
        bool operator!=(const Iterateur&) const;
        
        const T& operator*() const;

        Iterateur& operator++();
        Iterateur operator++(int);
        Iterateur& operator = (const Iterateur&);
        
      private:
        const ArbreAVL& arbre_associe;
        Noeud* courant;
        Pile<Noeud*> chemin;
        
      friend class ArbreAVL;
    };
};


//-----------------------------------------------------------------------------

template <class T>
ArbreAVL<T>::Noeud::Noeud(const T& c):
gauche(NULL), droite(NULL), equilibre(0), contenu(c)
{
}

template <class T>
ArbreAVL<T>::ArbreAVL() : racine(NULL)
{
}

template <class T>
ArbreAVL<T>::ArbreAVL(const ArbreAVL<T>& autre) : racine(NULL)
{
    copier(autre.racine, racine);
}

template <class T>
ArbreAVL<T>::~ArbreAVL()
{
    vider(racine);
}

template <class T>
bool ArbreAVL<T>::contient(const T& element) const
{
    Noeud *unNoeud = racine;

    while (unNoeud != NULL) {
        if (element == unNoeud->contenu) {
            return true;
        }
        if (element < unNoeud->contenu) {
            unNoeud = unNoeud->gauche;
        }
        else {
            unNoeud = unNoeud->droite;
        }
    } 
    return false;
}

template <class T>
void ArbreAVL<T>::inserer(const T& element)
{
    inserer(racine, element);
}

template <class T>
bool ArbreAVL<T>::inserer(Noeud*& noeud, const T& element)
{
    if(noeud==NULL)
    {
        noeud = new Noeud(element);
        return true;
    }
    if(element < noeud->contenu){
        if(inserer(noeud->gauche, element))
        {
            noeud->equilibre++;
            if(noeud->equilibre == 0) return false;
            if(noeud->equilibre == 1) return true;
            assert(noeud->equilibre == 2);
            if(noeud->gauche->equilibre == -1)
                rotationDroiteGauche(noeud->gauche);
            rotationGaucheDroite(noeud);
        }
        return false;
    }
    else if(noeud->contenu < element){
        if (inserer(noeud->droite, element))
        {
            noeud->equilibre--;
            if(noeud->equilibre ==  0) return false;
            if(noeud->equilibre == -1) return true;
            assert(noeud->equilibre == -2);
            if(noeud->droite->equilibre == 1)
                rotationGaucheDroite(noeud->droite);
            rotationDroiteGauche(noeud);
        }
        return false;
    }

    // element == noeud->contenu
    noeud->contenu = element;  // Mise à jour
    return false;
}

template <class T>
void ArbreAVL<T>::rotationGaucheDroite(Noeud*& racinesousarbre)
{
    Noeud *temp = racinesousarbre->gauche;
    int  ea = temp->equilibre;
    int  eb = racinesousarbre->equilibre;
    int  neb = -(ea>0 ? ea : 0) - 1 + eb;
    int  nea = ea + (neb < 0 ? neb : 0) - 1;

    temp->equilibre = nea;
    racinesousarbre->equilibre = neb;
    racinesousarbre->gauche = temp->droite;
    temp->droite = racinesousarbre;
    racinesousarbre = temp;
}

template <class T>
void ArbreAVL<T>::rotationDroiteGauche(Noeud*& racinesousarbre)
{
    Noeud *temp = racinesousarbre->droite;
    int ea = temp->equilibre;
    int eb = racinesousarbre->equilibre;
    int neb = -(ea<0 ? ea : 0) + 1 + eb;
    int nea = ea + (neb > 0 ? neb : 0) + 1;

    temp->equilibre = nea;
    racinesousarbre->equilibre = neb;
    racinesousarbre->droite = temp->gauche;
    temp->gauche = racinesousarbre;
    racinesousarbre = temp;
}

template <class T>
bool ArbreAVL<T>::vide() const
{
   
    if (racine == NULL) return true;
    return false;
}

template <class T>
void ArbreAVL<T>::vider(){
  vider(racine);
}

template <class T>
void ArbreAVL<T>::vider(Noeud*& noeud)
{
    if (noeud != NULL) {    
        if (noeud->gauche != NULL) {
            vider(noeud->gauche);
        }
        if (noeud->droite != NULL) {
            vider(noeud->droite);
        }
        delete noeud;
        noeud = NULL;
    }
}

template <class T>
void ArbreAVL<T>::copier(const Noeud* source, Noeud*& noeud) const
{
    if (source != NULL) {

        noeud = new Noeud(source->contenu);
        noeud->equilibre = source->equilibre;

        copier(source->gauche, noeud->gauche);
        copier(source->droite, noeud->droite);
    }
    else {
        noeud = NULL;
    }
}

template <class T>
int  ArbreAVL<T>::hauteur() const{

    int hauteur = 0;
    Noeud* unNoeud = racine;
    
    while (unNoeud != NULL) {
        unNoeud = (unNoeud->equilibre > 0 ? unNoeud->gauche : unNoeud->droite);
        ++hauteur;
    }
    return hauteur;
}

template <class T>
const T& ArbreAVL<T>::max(Noeud* n) const
{
    Noeud *unNoeud = n;
    if (n != NULL) {
        while (unNoeud->droite != NULL) {
            unNoeud = unNoeud->droite;
        }
        
        T& elemMax = unNoeud->contenu;
        return elemMax;
    }
}

//----------- L'enlèvement est optionnel (non requise pour le TP2) ----------//
template <class T>
void ArbreAVL<T>::enlever(const T& element)
{
    enlever(racine, element);
}

template <class T>
bool ArbreAVL<T>::enlever(Noeud*& noeud, const T& element)
{
    Noeud *unNoeud;

    if(element < noeud->contenu)
    {
        if(enlever(noeud->gauche, element))
        {
            --(noeud->equilibre); 
            if (noeud->equilibre == 0 ) return true;
            if (noeud->equilibre == -1) return false;
            assert(noeud->equilibre == -2);
            if (noeud->droite->equilibre == 1) 
                rotationGaucheDroite(noeud->droite);
            rotationDroiteGauche(noeud);
        }
        return false;
    }
    else if(element > noeud->contenu)
    {
        if(enlever(noeud->droite, element))
        {
            ++(noeud->equilibre);
            if (noeud->equilibre == 0) return true;
            if (noeud->equilibre == 1) return false;
            assert(noeud->equilibre == 2);
            if (noeud->gauche->equilibre == -1)
                rotationDroiteGauche(noeud->gauche);
            rotationGaucheDroite(noeud);
        }
        return false;
    }
    else  //if(element == noeud->contenu)
    {
                                                            
        if(noeud->gauche == NULL && noeud->droite != NULL)
        {   
            unNoeud = noeud;
            noeud = noeud->droite;
            delete unNoeud;
            unNoeud = NULL;    
            return true;
        }
        if(noeud->gauche != NULL && noeud->droite == NULL) {
            unNoeud = noeud;
            noeud = noeud->gauche;
            delete unNoeud;
            unNoeud = NULL;
            return true;
        }
        
        // si suppression d'une feuille
        if (noeud->gauche==NULL && noeud->droite==NULL)
        {
            delete noeud;
            noeud = NULL;
            return true;
        }
        else
       { 
           // si suppression d'un noeud avec deux enfants
            noeud->contenu = max(noeud->gauche);
            if(enlever(noeud->gauche, noeud->contenu)) {
                --(noeud->equilibre);
                if (noeud->equilibre == 0)  return true;
                if (noeud->equilibre == -1) return false;
                assert(noeud->equilibre == -2);
                if (noeud->droite->equilibre == 1) 
                    rotationGaucheDroite(noeud->droite);
                rotationDroiteGauche(noeud);
            }
            return false;
        }
    }
}
//----------- L'enlèvement est optionnel (non requise pour le TP2) ----------//

template <class T>
typename ArbreAVL<T>::Iterateur ArbreAVL<T>::debut() const
{
    Iterateur iter(*this);
    if (iter.arbre_associe.racine != NULL) {
        iter.courant = iter.arbre_associe.racine;

        while (iter.courant != NULL) {
            iter.chemin.empiler(iter.courant);
            iter.courant = iter.courant->gauche;
        }
        iter.courant = iter.chemin.depiler();
    }
    return iter;
}

template <class T>
typename ArbreAVL<T>::Iterateur ArbreAVL<T>::fin() const
{
    
    Iterateur iter(*this);
    iter.courant = NULL;

    return iter;
}

template <class T>
typename ArbreAVL<T>::Iterateur ArbreAVL<T>::rechercher(const T& e) const
{
    Iterateur iter(*this);

    while(iter.courant != NULL) {
        if (iter.courant->contenu == e) {
            return iter;
        }
        else if (iter.courant->contenu < e) {
            iter.courant = iter.courant->droite;
        }
        else {
            iter.chemin.empiler(iter.courant);
            iter.courant = iter.courant->gauche;
        }
    }
    return iter;
}

template <class T>
typename ArbreAVL<T>::Iterateur ArbreAVL<T>::rechercherEgalOuSuivant(const T& e) const
{
    Iterateur iter = this->rechercher(e);

    if (iter.courant == NULL && iter.chemin.vide() == false) {
        iter.courant = iter.chemin.depiler();
    }
    return iter;
}

template <class T>
typename ArbreAVL<T>::Iterateur ArbreAVL<T>::rechercherEgalOuPrecedent(const T& e) const
{
    Iterateur iter(*this);
    Noeud* precedent = NULL;

   while (iter.courant != NULL) {
       if (iter.courant->contenu == e) {
           return iter;
       }
       else if (iter.courant->contenu < e) {
           if (precedent == NULL || iter.courant->contenu > precedent->contenu) {
               precedent = iter.courant;
           }
           iter.courant = iter.courant->droite;
       }
       else {
           iter.chemin.empiler(iter.courant);
           iter.courant = iter.courant->gauche;       
       }
   }
   if (iter.courant == NULL && precedent != NULL) {
       iter.courant = precedent;
   }
    return iter;  
}

template <class T>
const T& ArbreAVL<T>::operator[](const Iterateur& iterateur) const
{
    assert(&iterateur.arbre_associe == this);
    assert(iterateur.courant);
    return iterateur.courant->contenu;
}

template <class T>
T& ArbreAVL<T>::operator[](const Iterateur& iterateur)
{
    assert(&iterateur.arbre_associe == this);
    assert(iterateur.courant);
    return iterateur.courant->contenu;
}

template <class T>
ArbreAVL<T>& ArbreAVL<T>::operator=(const ArbreAVL& autre) {
    if(this==&autre) return *this;
    vider();
    copier(autre.racine, racine);
    return *this;
}

//---------------
template <class T>
ArbreAVL<T>::Iterateur::Iterateur(const ArbreAVL& a)
 : arbre_associe(a), courant(NULL)
{   
    courant = arbre_associe.racine;
}

template <class T>
ArbreAVL<T>::Iterateur::Iterateur(const ArbreAVL<T>::Iterateur& a)
: arbre_associe(a.arbre_associe)
{
    courant = a.courant;
    chemin = a.chemin;
}

// Pré-incrément
template <class T>
typename ArbreAVL<T>::Iterateur& ArbreAVL<T>::Iterateur::operator++()
{
 
    if (courant != NULL) {
        if (courant->droite != NULL) {
            courant = courant->droite;
            while(courant->gauche != NULL) {
                chemin.empiler(courant);
                courant = courant->gauche;
            }
        }
        else {
            if (chemin.vide() == false) {
                courant = chemin.depiler();
            }
            else {
                courant = NULL;
            }
        }
    }
    return *this;
}

// Post-incrément
template <class T>
typename ArbreAVL<T>::Iterateur ArbreAVL<T>::Iterateur::operator++(int)
{
    Iterateur copie(*this);
    operator++();
    return copie;
}

template <class T>
ArbreAVL<T>::Iterateur::operator bool() const
{
    return courant!=NULL;
}

template <class T>
bool ArbreAVL<T>::Iterateur::operator!() const{
    return courant==NULL;
}

template <class T>
bool ArbreAVL<T>::Iterateur::operator==(const Iterateur& o) const{
    assert(&arbre_associe==&o.arbre_associe);
    return courant==o.courant;
}

template <class T>
bool ArbreAVL<T>::Iterateur::operator!=(const Iterateur& o) const{
    assert(&arbre_associe==&o.arbre_associe);
    return courant!=o.courant;
}

template <class T>
const T& ArbreAVL<T>::Iterateur::operator *() const
{
    assert(courant!=NULL);
    return courant->contenu;
}

template <class T>
typename ArbreAVL<T>::Iterateur& ArbreAVL<T>::Iterateur::operator = (const Iterateur& autre){
    assert(&arbre_associe==&autre.arbre_associe);
    courant = autre.courant;
    chemin = autre.chemin;
    return *this;
}

#endif


