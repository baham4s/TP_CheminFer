#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <train.h>
#include <moniteur_voie_unique.h>

/* Ajout d'un controle de l'utilisation du moniteur a l'aide de mutex */

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
  pthread_mutex_init(&moniteur->mutex, NULL);
  pthread_cond_init(&moniteur->cond_e, NULL);
  pthread_cond_init(&moniteur->cond_o, NULL);
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
    pthread_mutex_lock(&(moniteur->mutex));     // Verrouillage du moniteur
    while (moniteur->nb_train_e > 0 || moniteur->nb_train_o >= moniteur->nb){   // Vérification que nombre de trains en sortie est nul ou que l'entrée est inférieur au nombre de trains
        pthread_cond_wait(&(moniteur->cond_o), &(moniteur->mutex)); // Attente conditions satisfaites
    }
    moniteur->nb_train_o++;
    pthread_cond_signal(&(moniteur->cond_o));   // Réveil ouest
    pthread_mutex_unlock(&(moniteur->mutex));   // Déverrouillage du moniteur
}

extern void moniteur_voie_unique_sortie_est( moniteur_voie_unique_t * moniteur ) 
{
    pthread_mutex_lock(&(moniteur->mutex)); // Verrouillage du moniteur
    moniteur->nb_train_o--;
    if (moniteur->nb_train_o == 0) pthread_cond_signal(&(moniteur->cond_e));    // Réveil est
    if (moniteur->nb_train_o == moniteur->nb-1) pthread_cond_signal(&(moniteur->cond_o));   // Réveil ouest
    pthread_mutex_unlock(&(moniteur->mutex));   // Déverrouillage du moniteur
}

extern void moniteur_voie_unique_entree_est( moniteur_voie_unique_t * moniteur ) 
{
    pthread_mutex_lock(&(moniteur->mutex)); // Verrouillage du moniteur
    while (moniteur->nb_train_o > 0 || moniteur->nb_train_e >= moniteur->nb) { // Vérification que nombre de trains en sortie est nul ou que l'entrée est inférieur au nombre de trains
        pthread_cond_wait(&(moniteur->cond_e), &(moniteur->mutex)); // Attente conditions satisfaites
    }
    moniteur->nb_train_e++;
    pthread_cond_signal(&(moniteur->cond_e));   // Réveil
    pthread_mutex_unlock(&(moniteur->mutex));   // Déverrouillage du moniteur
}

extern void moniteur_voie_unique_sortie_ouest( moniteur_voie_unique_t * moniteur ) 
{
    pthread_mutex_lock(&(moniteur->mutex)); // Verrouillage du moniteur
    moniteur->nb_train_e--;
    if (moniteur->nb_train_e == 0) pthread_cond_signal(&(moniteur->cond_o));    // Réveil ouest
    if (moniteur->nb_train_e == moniteur->nb-1) pthread_cond_signal(&(moniteur->cond_e));   // Réveil est
    pthread_mutex_unlock(&(moniteur->mutex));   // Déverrouillage du moniteur
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
    pthread_mutex_lock(&(moniteur->mutex)); // Verrouillage du moniteur
    int r = voie_unique_extraire(moniteur->voie_unique, (*train), zone, train_sens_get(train));
    pthread_mutex_unlock(&(moniteur->mutex));   // Déverrouillage du moniteur
    return r;
}

extern
int moniteur_voie_unique_inserer( moniteur_voie_unique_t * moniteur , train_t * train , zone_t zone ) 
{ 
    pthread_mutex_lock(&(moniteur->mutex)); // Verrouillage du moniteur
    int r =  voie_unique_inserer(moniteur->voie_unique, (*train), zone, train_sens_get(train));
    pthread_mutex_unlock(&(moniteur->mutex));   // Déverrouillage du moniteur
    return r;
}

/*
 * Ajout d'un mutex sur l'affichage dans le trafic1
 * */
extern void moniteur_voie_unique_print(moniteur_voie_unique_t* moniteur, affFunc aff) {
    voie_unique_t * voie = moniteur_voie_unique_get(moniteur);
    pthread_mutex_lock(&(moniteur->mutex)); // Verrouillage du moniteur
    aff(voie);
    pthread_mutex_unlock(&(moniteur->mutex));   // Déverrouillage du moniteur
}