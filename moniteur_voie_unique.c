// TODO :   Commentaire *.c && *.h
//          Trafic 2 && 3

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include <sens.h>
#include <train.h>
#include <moniteur_voie_unique.h>

/*---------- MONITEUR ----------*/
void lck(moniteur_voie_unique_t *m){
    pthread_mutex_lock(&(m->mutex));
}

void ulck(moniteur_voie_unique_t *m) {
    pthread_mutex_unlock(&(m->mutex));
}

extern moniteur_voie_unique_t * moniteur_voie_unique_creer( const train_id_t nb )
{
  moniteur_voie_unique_t * moniteur = NULL ; 

  /* Creation structure moniteur */
  if( ( moniteur = malloc( sizeof(moniteur_voie_unique_t) ) ) == NULL  )
    {
      fprintf( stderr , "moniteur_voie_unique_creer: débordement memoire (%lu octets demandes)\n" ,
	       sizeof(moniteur_voie_unique_t) ) ;
      return(NULL) ; 
    }

  /* Creation de la voie */
  if( ( moniteur->voie_unique = voie_unique_creer() ) == NULL )
    return(NULL) ; 
  
  /* Initialisations du moniteur */

  moniteur->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
  moniteur->cond_e = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
  moniteur->cond_o = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
  moniteur->nb = nb;
  moniteur->nb_train_e = 0;
  moniteur->nb_train_o = 0;

  return(moniteur) ; 
}

extern int moniteur_voie_unique_detruire( moniteur_voie_unique_t ** moniteur )
{
  int noerr ; 

  /* Destructions des attributs du moniteur */

  pthread_mutex_destroy(&((*moniteur)->mutex));
  pthread_cond_destroy(&((*moniteur)->cond_e));
  pthread_cond_destroy(&((*moniteur)->cond_o ));

  /* Destruction de la voie */
  if( ( noerr = voie_unique_detruire( &((*moniteur)->voie_unique) ) ) )
    return(noerr) ;

  /* Destruction de la structure du moniteur */
  free( (*moniteur) );

  (*moniteur) = NULL ; 

  return(0) ; 
}

extern void moniteur_voie_unique_entree_ouest( moniteur_voie_unique_t * moniteur ) 
{
    lck(moniteur);
    while (moniteur->nb_train_e > 0 || moniteur->nb_train_o >= moniteur->nb){
        pthread_cond_wait(&(moniteur->cond_o), &(moniteur->mutex));
    }
    moniteur->nb_train_o++;
    pthread_cond_signal(&(moniteur->cond_o));
    ulck(moniteur);
}

extern void moniteur_voie_unique_sortie_est( moniteur_voie_unique_t * moniteur ) 
{
    lck(moniteur);
    moniteur->nb_train_o--;
    if (moniteur->nb_train_o == 0) pthread_cond_signal(&(moniteur->cond_e));
    if (moniteur->nb_train_o == moniteur->nb-1) pthread_cond_signal(&(moniteur->cond_o));
    ulck(moniteur);
}

extern void moniteur_voie_unique_entree_est( moniteur_voie_unique_t * moniteur ) 
{
    lck(moniteur);
    while (moniteur->nb_train_o > 0 || moniteur->nb_train_e >= moniteur->nb) {
        pthread_cond_wait(&(moniteur->cond_e), &(moniteur->mutex));
    }
    moniteur->nb_train_e++;
    pthread_cond_signal(&(moniteur->cond_e));
    ulck(moniteur);
}

extern void moniteur_voie_unique_sortie_ouest( moniteur_voie_unique_t * moniteur ) 
{
    lck(moniteur);
    moniteur->nb_train_e--;
    if (moniteur->nb_train_e == 0) pthread_cond_signal(&(moniteur->cond_o));
    if (moniteur->nb_train_e == moniteur->nb-1) pthread_cond_signal(&(moniteur->cond_e));
    ulck(moniteur);
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
    return(moniteur->nb);
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
