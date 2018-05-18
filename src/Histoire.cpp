/*
 * Histoire.cpp
 *
 *  Created on: Mar 18, 2017
 *      Author: Bruno Malenfant
 */

#include "Histoire.h"




#include <sstream>

Histoire::Histoire(void) {
}

Histoire::Histoire(string a_titre) :
   _titre( a_titre )
{
}

Histoire::Histoire(const Histoire& a_histoire) :
    _titre( a_histoire._titre ),
    _mots( a_histoire._mots )
{
}

Histoire::~Histoire(void) {
}

string Histoire::titre(void) const {
    return _titre;
}

void Histoire::ajouterMot(string a_mot) {
    _mots.push_back( a_mot );
}

vector<string>::const_iterator Histoire::begin(void) const {
    return _mots.begin();
}

vector<string>::const_iterator Histoire::end(void) const {
    return _mots.end();
}

bool estCaractereMot( char c ) {
    return ( 'A' <= c && c <= 'Z' ) ||
           ( 'a' <= c && c <= 'z' ) ||
           ( '-' == c );
}

vector<Histoire *> * extraireHistoires(const DocumentXML& a_document) {
    vector< Histoire * > * resultat = new vector< Histoire * >();

    Element * courrant = a_document.racine();
    vector< Contenu * >::const_iterator it = courrant->begin();

    // trouver <magasine>
    while( ! ( * it )->estElement() ) ++ it;
    courrant = ( Element * )( * it );

    it = courrant->begin();

    // trouver <contenus>
    while( ! ( * it )->estElement() ) ++ it;
    courrant = ( Element * )( * it );

    it = courrant->begin();
    vector< Contenu * >::const_iterator fin = courrant->end();

    for( ; it < fin; ++ it ) {
        if( ( * it )->estElement() ) {
            Element * histoire = ( Element * )( * it );

            string titre = histoire->attribut( string( "titre" ) );

            try {
                titre += " (partie " + histoire->attribut( string( "partie" ) ) + ")";
            } catch( AttributNonDefinie & a_e ) {
                // rien
            }

            Histoire * nouvelle = new Histoire( titre );

            stringstream texte( histoire->texte() );
            stringstream accumulateur;
            char c;

            while( ! texte.eof() ) {
                do {
                    texte.get( c );
                } while( ! estCaractereMot( c ) && ! texte.eof() );

                while( estCaractereMot( c ) && ! texte.eof() ) {
                    accumulateur << c;
                    texte.get( c );
                }

                string mot = accumulateur.str();
                accumulateur.str( "" );

                if( mot.length() != 0 ) {
                    nouvelle->ajouterMot( mot );
                }
            }

            resultat->push_back( nouvelle );
        }
    }

    return resultat;
}

ostream & operator << ( ostream & a_out, const Histoire & a_histoire ){
    int compteur = 0;

    a_out << a_histoire._titre << endl << endl;

    for( string mot : a_histoire ) {
        a_out << mot << ' ';
        compteur += mot.length();
        if( compteur > TAILLE_LIGNE ) {
            a_out << endl;
            compteur -= TAILLE_LIGNE;
        }
    }

    return a_out;
}
