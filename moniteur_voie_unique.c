#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include <sens.h>
#include <train.h>
#include <moniteur_voie_unique.h>

/*---------- MONITEUR ----------*/

extern moniteur_voie_unique_t * moniteur_voie_unique_creer( const train_id_t nb )
{
  moniteur_voie_unique_t * moniteur = NULL ; 

  /* Creation structure moniteur */
  if( ( moniteur = malloc( sizeof(moniteur_voie_unique_t) ) ) == NULL  )
    {
      fprintf( stderr , "moniteur_voie_unique_creer: debordement memoire (%lu octets demandes)\n" , 
	       sizeof(moniteur_voie_unique_t) ) ;
      return(NULL) ; 
    }

  /* Creation de la voie */
  if( ( moniteur->voie_unique = voie_unique_creer() ) == NULL )
    return(NULL) ; 
  
  /* Initialisations du moniteur */

  /***********/
  /* A FAIRE */
  /***********/

  return(moniteur) ; 
}

extern int moniteur_voie_unique_detruire( moniteur_voie_unique_t ** moniteur )
{
  int noerr ; 

  /* Destructions des attribiuts du moniteur */

  /***********/
  /* A FAIRE */
  /***********/

  /* Destruction de la voie */
  if( ( noerr = voie_unique_detruire( &((*moniteur)->voie_unique) ) ) )
    return(noerr) ;

  /* Destruction de la strcuture du moniteur */
  free( (*moniteur) );

  (*moniteur) = NULL ; 

  return(0) ; 
}

extern void moniteur_voie_unique_entree_ouest( moniteur_voie_unique_t * moniteur ) 
{
  /***********/
  /* A FAIRE */
  /***********/
}

extern void moniteur_voie_unique_sortie_est( moniteur_voie_unique_t * moniteur ) 
{
  /***********/
  /* A FAIRE */
  /***********/
}

extern void moniteur_voie_unique_entree_est( moniteur_voie_unique_t * moniteur ) 
{
  /***********/
  /* A FAIRE */
  /***********/
}

extern void moniteur_voie_unique_sortie_ouest( moniteur_voie_unique_t * moniteur ) 
{
  /***********/
  /* A FAIRE */
  /***********/
}

/*
 * Fonctions set/get 
 */

extern 
voie_unique_t * moniteur_voie_unique_get( moniteur_voie_unique_t * const moniteur )
{
  return( moniteur->voie_unique ) ; 
}


extern 
train_id_t moniteur_max_trains_get( moniteur_voie_unique_t * const moniteur )
{
  /***********/
  /* A FAIRE */
  /***********/
  return( 1 ) ; /* valeur arbitraire ici */
}

/*
 * Fonction de deplacement d'un train 
 */

extern
int moniteur_voie_unique_extraire( moniteur_voie_unique_t * moniteur , train_t * train , zone_t zone  ) 
{
  return( voie_unique_extraire( moniteur->voie_unique, 
				(*train), 
				zone , 
				train_sens_get(train) ) ) ; 
}

extern
int moniteur_voie_unique_inserer( moniteur_voie_unique_t * moniteur , train_t * train , zone_t zone ) 
{ 
  return( voie_unique_inserer( moniteur->voie_unique, 
			       (*train), 
			       zone, 
			       train_sens_get(train) ) ) ;
}
